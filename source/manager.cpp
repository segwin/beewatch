//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "manager.h"

#include "util/logging.h"
#include "util/string.h"
#include "util/time.hpp"

#include <chrono>
#include <iostream>

namespace beewatch
{
    
    //==============================================================================
    Manager::Manager()
    {
        // Initialise DHT11
        _climateSensor = std::make_unique<hw::DHTxx>(hw::DHTxx::Type::DHT22, io::GPIO::claim(16));

        // Initialise HX711
        //hx711 = std::make_unique<hw::HX711>(io::GPIO::claim(5), io::GPIO::claim(6));
    }
    
    //==============================================================================
    void Manager::init(int argc, char * argv[])
    {
        std::map<std::string, std::string> args;
        std::string exeName = argv[0];

        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            if (arg == "-w" || arg == "--web-root")
            {
                if (i + 1 < argc)
                {
                    i++;
                    args["web-root"] = argv[i];
                }
                else
                {
                    std::cerr << "Expected path after \"" << arg << "\" option!" << std::endl;
                    printUsage(exeName);
                }
            }
            else if (arg == "-h" || "--help")
            {
                printUsage(exeName);
                exit(0);
            }
        }

        if (args["web-root"].empty())
        {
            std::cerr << "No argument for \"--web-root\" provided!" << std::endl;
            printUsage(exeName);
        }
        else
        {
            _webServer = std::make_unique<http::Server>(80, args["web-root"], *this);
        }
    }

    void Manager::printUsage(std::string exeName)
    {
        std::cout << exeName << " [-w|--web /path/to/web/dist] [-h|--help]" << std::endl
                  << std::endl
                  << "Arguments:" << std::endl
                  << "    -w, --web /path/to/web/dist       (Required) Specify path to web page build directory" << std::endl
                  << "    -h, --help                        Display this help message" << std::endl;
    }

    //==============================================================================
    std::string Manager::getName() const
    {
        std::shared_lock<std::shared_mutex> readLock(_attrMutex);
        return _name;
    }

    void Manager::setName(std::string name)
    {
        std::unique_lock<std::shared_mutex> writeLock(_attrMutex);
        _name = name;
    }

    //==============================================================================
    std::map<time_t, ClimateData<double>> Manager::getClimateSamples(time_t since) const
    {
        std::shared_lock<std::shared_mutex> readLock(_attrMutex);

        // TODO: Implement storage class for sample data
        if (since > g_timeReal.now())
            return {};
        else
            return { { g_timeReal.now(), _climateSensor->read() } };
    }

    //==============================================================================
    void Manager::ctrlLoop()
    {
        while (1)
        {
            auto data = _climateSensor->read();

            g_logger.print(Logger::Info, "Humidity: " + string::fromNumber(data.humidity) + " %");
            g_logger.print(Logger::Info, "Temperature: " + string::fromNumber(data.temperature) + " deg Celsius");

            // Measure mass
            //auto mass = hx711->read();
            //
            //g_logger.print(Logger::Info, "Mass: " + numToStr(mass) + " kg");

            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

} // namespace beewatch
