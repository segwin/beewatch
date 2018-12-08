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
    Server::Server(int port, std::string webRoot, IManager& manager)
        : _port(port), _manager(manager), _changingState(false)
    {
        setWebRoot(webRoot);
        restart();
    }

    Server::~Server()
    {
        stop();
    }
    
    //==============================================================================
    void Server::setWebRoot(std::string webRootPath)
    {
        _webRoot = webRootPath;

        // Preload all web resources in memory
        _webResources.clear();

        std::vector<std::string> resourceFiles = file::listContents(_webRoot, true);
        for (const auto& resourceFile : resourceFiles)
        {
            _webResources[resourceFile] = file::readText(_webRoot + resourceFile);
        }
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

    void Server::listen()
    {
        std::unique_lock<std::mutex> lock(_mutex);
        
        // REST API handler
        IManager& manager = _manager;

        auto answerAPIRequest = [&manager](const http_request& request) {
            g_logger.print(Logger::Debug, "Received " + request.method() + " request");

            // Compare uri to API values & answer accordingly
            auto answer = json::value::object();
            auto uri = getRequestURI(request);

            if (request.method() == methods::GET)
            {
                if (uri == "/api/data/climate")
                {
                    auto msg = request.extract_json().get();
                    time_t since = 0;

                    if (msg.is_object())
                    {
                        auto& jsonMsg = msg.as_object();
                        for (const auto& node : jsonMsg)
                        {
                            if (node.first == "since")
                            {
                                try
                                {
                                    since = node.second.as_integer();
                                    break;
                                }
                                catch (const json_exception& e)
                                {
                                    answer["error"] = json::value::string("Caught error while interpreting \"since\" "
                                                                          "parameter in \"GET /data/climate\" request "
                                                                          "(node: " + node.second.as_string());

                                    g_logger.print(Logger::Error, answer["error"].as_string());

                                    return answer;
                                }
                            }
                        }
                    }

                    answer["interior"] = json::value::object();

                    auto climateSamples = manager.getClimateSamples(since);
                    for (auto& sample : climateSamples)
                    {
                        answer["interior"][sample.first] = json::value::object();

                        answer["interior"]["temperature"] = sample.second.temperature;
                        answer["interior"]["humidity"] = sample.second.humidity;
                    }

                    return answer;
                }
                else if (uri == "/api/name")
                {
                    answer["name"] = json::value::string(manager.getName());
                    return answer;
                }
                else if (uri == "/api/version")
                {
                    answer["version"] = json::value::string(NAME_VERSION);
                    
                    answer["major"] = json::value::string(VERSION_MAJOR);
                    answer["minor"] = json::value::string(VERSION_MINOR);
                    answer["patch"] = json::value::string(VERSION_PATCH);
                    answer["revision"] = json::value::string(VERSION_REVISION);
                    
                    return answer;
                }
            }
            else if (request.method() == methods::POST)
            {
                if (uri == "/api/name")
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
                                                                          node.second.as_string());

                                    g_logger.print(Logger::Error, answer["error"].as_string());

                                    return answer;
                                }
                            }
                        }
                    }

                    if (!name.empty())
                    {
                        // Set new name and return it in response
                        manager.setName(name);
                        answer["name"] = json::value::string(manager.getName());

                        return answer;
                    }
                    else
                    {
                        answer["error"] = json::value::string("No value for \"name\" provided in \"POST /name\" request!");
                        g_logger.print(Logger::Error, answer["error"].as_string());

                        return answer;
                    }
                }
            }

            answer["error"] = json::value::string("Invalid API request: \"" + request.to_string() + "\"");
            g_logger.print(Logger::Error, answer["error"].as_string());

            return answer;
        };
        
        // Web resource request handler
        std::string& webRoot = _webRoot;
        const auto& webResources = _webResources;

        auto answerResourceRequest = [&webRoot, &webResources](const http_request& request) {
            http_response answer;
            
            auto uri = getRequestURI(request);

            // Only GET requests are expected for web resources
            if (request.method() != methods::GET)
            {
                answer.set_status_code(status_codes::BadRequest);
                answer.set_body("<h1>400 - Requested an invalid resource (\"" + request.method() + " " + uri + "\")",
                                "text/html; charset=utf-8");

                return answer;
            }

            // Compare uri to recognised resources & answer accordingly
            for (const auto& mappedResource : webResources)
            {
                if (string::tolower(uri) == string::tolower(mappedResource.first))
                {
                    answer.set_status_code(status_codes::OK);
                    answer.set_body(file::readText(webRoot + mappedResource.first), "text/html; charset=utf-8");

                    return answer;
                }
            }
            
            answer.set_status_code(status_codes::NotFound);
            answer.set_body("<h1>404 - The requested resource (\"" + uri + "\") was not found</h1>",
                            "text/html; charset=utf-8");

            return answer;
        };

        // General request handler
        auto answerRequest = [&answerAPIRequest, &answerResourceRequest](http_request request) {
            auto uri = getRequestURI(request);

            if (uri.size() >= 4 && uri.substr(0, 4) == "/api")
            {
                json::value answer = answerAPIRequest(request);
                status_code status = status_codes::OK;
                
                if (answer.has_field("error"))
                {
                    status = status_codes::BadRequest;
                }

                g_logger.print(Logger::Debug, "Response: " + answer.as_string());
                request.reply(status, answer);
            }
            else
            {
                http_response answer = answerResourceRequest(request);

                g_logger.print(Logger::Debug, "Response: " + answer.to_string());
                request.reply(answer);
            }
        };
        

        // Create listener and add handler for supported methods
        http_listener listener("http://0.0.0.0:" + std::to_string(_port) + "/");

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
