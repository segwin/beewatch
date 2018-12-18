//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "http/server.h"

#include "util/logging.h"
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
    Server::Server(int port, IManager& manager)
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
            g_logger.print(
                Logger::Warning,
                "Requested web server restart, but state change already in progress. Ignoring request."
            );

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
            g_logger.print(
                Logger::Warning,
                "Requested web server stop, but state change already in progress. Ignoring request."
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
            g_logger.print(Logger::Debug, "Received " + request.method() + " request");

            // Compare uri to API values & answer accordingly
            auto answer = json::value::object();
            auto uri = getRequestURI(request);

            if (request.method() == methods::GET)
            {
                if (uri == "data/climate")
                {
                    auto query = getRequestQuery(request);
                    time_t since = 0;

                    if (query.find("since") != query.end())
                    {
                        try
                        {
                            since = std::stoi(query.at("since"));
                        }
                        catch (const std::invalid_argument)
                        {
                            answer["error"] = json::value::string("Caught error while interpreting \"since\" "
                                                                  "parameter in \"GET /data/climate\" request "
                                                                  "(got \"?since=" + query.at("since") + "\"");

                            g_logger.print(Logger::Error, answer["error"].serialize());

                            request.reply(status_codes::BadRequest, answer);
                            return;
                        }
                    }

                    // Create JSON array from map of climate samples
                    auto samples = _manager.getClimateSamples(since);

                    answer["interior"] = json::value::object(true);
                    answer["interior"]["timestamps"] = json::value::array(samples.size());
                    answer["interior"]["samples"] = json::value::array(samples.size());

                    size_t index = 0;
                    for (auto& sample : samples)
                    {
                        answer["interior"]["timestamps"][index] = json::value::number(sample.first);

                        answer["interior"]["samples"][index] = json::value::object({
                                { "temperature", sample.second.temperature },
                                { "humidity", sample.second.humidity }
                            }, true);

                        ++index;
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
            else if (request.method() == methods::POST)
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
                                    answer["error"] = json::value::string("Caught error while interpreting \"name\" "
                                                                          "parameter in \"POST /name\" request (node: " +
                                                                          node.second.serialize());

                                    g_logger.print(Logger::Error, answer["error"].serialize());

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
                        answer["error"] = json::value::string("No value for \"name\" provided in \"POST /name\" request!");
                        g_logger.print(Logger::Error, answer["error"].serialize());

                        request.reply(status_codes::BadRequest, answer);
                        return;
                    }
                }
            }

            answer["error"] = json::value::string("Invalid API request: \"" + request.to_string() + "\"");
            g_logger.print(Logger::Error, answer["error"].serialize());

            request.reply(status_codes::NotFound, answer);
            return;
        };
        

        // Create listener and add handler for supported methods
        http_listener listener("http://0.0.0.0:" + std::to_string(_port) + "/api/v1/");

        listener.support(methods::GET,  answerRequest);
        listener.support(methods::POST, answerRequest);

        // Loop until stop signal received
        try
        {
            listener.open()
                    .then([](){ g_logger.print(Logger::Debug, "Starting Server listener"); })
                    .wait();

            _stopCondition.wait(lock);
        }
        catch (const std::exception& e)
        {
            std::string errMsg = "Caught exception in REST API's listener thread: ";
            errMsg += e.what();

            g_logger.print(Logger::Error, errMsg);
        }
    }

} // namespace beewatch::http
