//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "rest/rest_api.h"

#include "logging.h"
#include "str.h"
#include "version.h"

#include <cpprest/http_listener.h>

namespace beewatch::rest
{

    using namespace web;
    using namespace web::http;
    using namespace web::http::experimental::listener;
    using namespace web::json;

    //==============================================================================
    RestAPI::RestAPI(int port, Manager& manager)
        : _port(port), _manager(manager), _changingState(false)
    {
        restart();
    }

    RestAPI::~RestAPI()
    {
        stop();
    }

    //==============================================================================
    void RestAPI::restart()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        // Only allow one state change at a time
        if (_changingState)
        {
            g_logger.print(
                Logger::Warning,
                "Requested REST API listener restart, but state change already in progress. Ignoring request."
            );

            return;
        }

        _changingState = true;

        // Stop listener if currently running
        stop(lock);

        // Start listener
        _thread = std::make_unique<std::thread>(&RestAPI::listen, this);
        _changingState = false;
    }

    void RestAPI::stop()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        // Only allow one state change at a time
        if (_changingState)
        {
            g_logger.print(
                Logger::Warning,
                "Requested REST API listener stop, but state change already in progress. Ignoring request."
            );

            return;
        }

        _changingState = true;

        // Stop listener if currently running
        stop(lock);

        _changingState = false;
    }

    void RestAPI::stop(std::unique_lock<std::mutex>& lock)
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
    void RestAPI::listen()
    {
        std::unique_lock<std::mutex> lock(_mutex);

        // Create handler method based on:
        // https://mariusbancila.ro/blog/2017/11/19/revisited-full-fledged-client-server-example-with-c-rest-sdk-2-10/
        auto& manager = _manager;
        auto handleRequest = [&manager](http_request request) {
            g_logger.print(Logger::Debug, "Received " + request.method() + " request");

            const auto relative_uri = request.relative_uri();
            auto path = tolower(relative_uri.path());

            // Ensure path ends with a separator
            if (path.back() != '/')
                path.push_back('/');

            // Compare path to API values & answer accordingly
            bool requestIsValid = false;
            auto answer = json::value::object();

            if (request.method() == "GET")
            {
                if (path == "/climate_data")
                {
                    requestIsValid = true;

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
                                    g_logger.print(
                                        Logger::Error,
                                        "Caught error while interpreting \"since\" parameter in "
                                        "\"GET /climate_data\" REST request (node: " + node.second.as_string()
                                    );
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

                }
                else if (path == "/name")
                {
                    answer["name"] = json::value::string(manager.getName());
                    requestIsValid = true;
                }
            }
            else if (request.method() == "POST")
            {
                if (path == "/name")
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
                                    g_logger.print(
                                        Logger::Error,
                                        "Caught error while interpreting \"name\" parameter in "
                                        "\"PUT /name\" REST request (node: " + node.second.as_string()
                                    );
                                }
                            }
                        }
                    }

                    if (!name.empty())
                    {
                        requestIsValid = true;

                        // Set new name and return it in response
                        manager.setName(name);
                        answer["name"] = json::value::string(manager.getName());
                    }
                }
            }

            if (!requestIsValid)
            {
                g_logger.print(Logger::Warning, "Received invalid request through REST API: " + request.to_string());
            }

            g_logger.print(Logger::Debug, "Response: " + answer.as_string());
            request.reply(requestIsValid ? status_codes::OK : status_codes::BadRequest, answer);
        };

        // Create listener and add handler for supported methods
        http_listener listener("http://localhost");

        listener.support(methods::GET,  handleRequest);
        listener.support(methods::POST, handleRequest);

        // Loop until stop signal received
        try
        {
            listener.open()
                    .then([](){ g_logger.print(Logger::Debug, "Starting RestAPI listener"); })
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

} // namespace beewatch::rest
