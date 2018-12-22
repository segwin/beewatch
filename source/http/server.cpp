//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "http/server.h"

#include "global/logging.h"
#include "util/file.h"
#include "util/string.h"
#include "version.h"

#include <cpprest/http_listener.h>

#include <fstream>

namespace beewatch::http
{

    using namespace web;
    using namespace web::http;
    using namespace web::http::experimental::listener;
    using namespace web::json;

    //==============================================================================
    Server::Server(IManager& manager, uint16_t port)
        : _port(port), _manager(manager), _changingState(false)
    {
        restart();
    }

    Server::~Server()
    {
        stop();
    }

    //==============================================================================
    void Server::restart()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        // Only allow one state change at a time
        if (_changingState)
        {
            g_logger.warning("Requested web server restart, but state change already "
                             "in progress. Ignoring request.");

            return;
        }

        _changingState = true;

        // Stop listener if currently running
        stop(lock);

        // Start listener
        _thread = std::make_unique<std::thread>(&Server::listen, this);
        _changingState = false;
    }

    void Server::stop()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        // Only allow one state change at a time
        if (_changingState)
        {
            g_logger.warning("Requested web server stop, but state change already in "
                             "progress. Ignoring request."
            );

            return;
        }

        _changingState = true;

        // Stop listener if currently running
        stop(lock);

        _changingState = false;
    }

    void Server::stop(std::unique_lock<std::mutex>& lock)
    {
        if (_thread)
        {
            _stopCondition.notify_one();
            _stopCondition.wait(lock);

            _thread->join();
            _thread = nullptr;
        }
    }

    //==============================================================================
    inline static std::string getRequestURI(const http_request& request)
    {
        const auto relative_uri = request.relative_uri();
        auto uri = string::tolower(relative_uri.path());

        // Remove root separator
        if (!uri.empty() && uri.front() == '/')
            uri.erase(0, 1);

        // Remove trailing slashes
        while (!uri.empty() && uri.back() == '/')
            uri.pop_back();

        return uri;
    }

    inline static std::map<std::string, std::string> getRequestQuery(const http_request& request)
    {
        const auto relative_uri = request.relative_uri();
        return relative_uri.split_query(relative_uri.query());
    }

    void Server::listen()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        
        // REST API handler
        auto answerRequest = [this](http_request request) {
            g_logger.debug("Received " + request.method() + " request");

            // Compare uri to API values & answer accordingly
            auto answer = json::value::object();
            auto uri = getRequestURI(request);

            if (request.method() == methods::GET)
            {
                if (uri == "data/climate")
                {
                    auto query = getRequestQuery(request);
                    int64_t since = 0;

                    if (query.find("since") != query.end())
                    {
                        try
                        {
                            since = std::stoi(query.at("since"));
                        }
                        catch (const std::invalid_argument&)
                        {
                            std::string errMsg = "Caught error while interpreting \"since\" "
                                                 "parameter in \"GET /data/climate\" request "
                                                 "(got \"?since=" + query.at("since") + "\"";

                            answer["error"] = json::value::string(errMsg);
                            g_logger.error(errMsg);

                            request.reply(status_codes::BadRequest, answer);
                            return;
                        }
                    }

                    // Create JSON array from map of climate samples
                    auto sensorIDs = _manager.getClimateSensorIDs();

                    for (const auto& sensorID : sensorIDs)
                    {
                        auto samples = _manager.getClimateSamples(sensorID, since);

                        answer[sensorID] = json::value::object(true);
                        answer[sensorID]["timestamps"] = json::value::array(samples.size());
                        answer[sensorID]["samples"] = json::value::array(samples.size());

                        size_t index = 0;
                        for (auto& sample : samples)
                        {
                            answer[sensorID]["timestamps"][index] = json::value::number((int64_t)sample.first);

                            answer[sensorID]["samples"][index] = json::value::object({
                                    { "temperature", sample.second.temperature },
                                    { "humidity", sample.second.humidity }
                                }, true);

                            ++index;
                        }
                    }

                    request.reply(status_codes::OK, answer);
                    return;
                }
                else if (uri == "name")
                {
                    answer["name"] = json::value::string(_manager.getName());

                    request.reply(status_codes::OK, answer);
                    return;
                }
                else if (uri == "version")
                {
                    answer["version"] = json::value::string(NAME_VERSION);
                    
                    answer["major"] = json::value::string(VERSION_MAJOR);
                    answer["minor"] = json::value::string(VERSION_MINOR);
                    answer["patch"] = json::value::string(VERSION_PATCH);
                    answer["revision"] = json::value::string(VERSION_REVISION);
                    
                    request.reply(status_codes::OK, answer);
                    return;
                }
            }
            else if (request.method() == methods::PUT)
            {
                if (uri == "name")
                {
                    // Get new name, only set requestIsValid if found
                    auto msg = request.extract_json().get();
                    std::string name;

                    if (msg.is_object())
                    {
                        auto& jsonMsg = msg.as_object();
                        for (const auto& node : jsonMsg)
                        {
                            if (node.first == "name")
                            {
                                try
                                {
                                    name = node.second.as_string();
                                    break;
                                }
                                catch (const json_exception& e)
                                {
                                    std::string errMsg = "Caught error while interpreting \"name\" "
                                                         "parameter in \"PUT /name\" request (node: " +
                                                         node.second.serialize();

                                    answer["error"] = json::value::string(errMsg);
                                    g_logger.error(errMsg);

                                    request.reply(status_codes::BadRequest, answer);
                                    return;
                                }
                            }
                        }
                    }

                    if (!name.empty())
                    {
                        // Set new name and return it in response
                        _manager.setName(name);
                        answer["name"] = json::value::string(_manager.getName());

                        request.reply(status_codes::OK, answer);
                        return;
                    }
                    else
                    {
                        std::string errMsg = "No value for \"name\" provided in \"PUT /name\" request!";

                        answer["error"] = json::value::string(errMsg);
                        g_logger.error(errMsg);

                        request.reply(status_codes::BadRequest, answer);
                        return;
                    }
                }
            }
            else if (request.method() == methods::DEL)
            {
                if (uri == "data/climate")
                {
                    _manager.clearClimateData();

                    request.reply(status_codes::OK);
                    return;
                }
            }

            std::string errMsg = "Invalid API request: \"" + request.to_string() + "\"";

            answer["error"] = json::value::string(errMsg);
            g_logger.error(errMsg);

            request.reply(status_codes::NotFound, answer);
            return;
        };
        

        // Create listener and add handler for supported methods
        http_listener listener("http://0.0.0.0:" + std::to_string(_port) + "/api/v1/");

        listener.support(methods::GET, answerRequest);
        listener.support(methods::PUT, answerRequest);
        listener.support(methods::DEL, answerRequest);

        // Loop until stop signal received
        try
        {
            listener.open()
                    .then([](){ g_logger.debug("Starting Server listener"); })
                    .wait();

            _stopCondition.wait(lock);
        }
        catch (const std::exception& e)
        {
            g_logger.error("Caught exception in REST API's listener thread: " + std::string(e.what()));
        }
    }

} // namespace beewatch::http
