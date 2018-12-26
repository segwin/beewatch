//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "global/manager.h"

#include "global/logging.h"
#include "global/time.hpp"
#include "util/string.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <set>
#include <sstream>

namespace beewatch
{

    //==============================================================================
    class Argument
    {
    public:
        /**
         * @brief Declare argument with a long name (--long-name) and an optional
         *        1-character synonym (-s)
         */
        Argument(std::string name, std::string description,
                 std::string expectedArg = "", char shortName = '\0')
            : name(std::string("--") + name),
              shortName(shortName == '\0' ? "" : std::string("-") + shortName),
              description(description),
              expectedArg(expectedArg)
        {
        }

        /**
         * @brief Compare two Argument objects to allow placement in a map
         */
        bool operator<(const Argument& arg) const
        {
            return name < arg.name;
        }

        /**
         * @brief Compare the given argument string with this argument
         */
        bool operator==(std::string arg) const
        {
            return arg == name || (!shortName.empty() && arg == shortName);
        }

        /**
         * @brief Return short argument usage string
         */
        std::string helpShort() const
        {
            // Build usage string using the following format:
            //  -s, --long "required arg"
            std::string usage;

            if (!shortName.empty())
                usage += shortName + " ";

            usage += name + " ";

            if (!expectedArg.empty())
                usage += "\"" + expectedArg + "\" ";

            return usage;
        }

        /**
         * @brief Return argument help message with description
         */
        std::string helpLong() const
        {
            // Append description to short description, aligned to the 36th character:
            //  -s, --long "required arg"           Argument description message
            std::ostringstream oss;

            oss << std::setw(36) << std::left << helpShort() << description;

            return oss.str();
        }

        const std::string name;
        const std::string shortName;
        const std::string description;
        const std::string expectedArg;
    };

    //==============================================================================
    Manager::Manager()
    {
        // Initialise DHT11
        // TODO: Support a dynamic configuration eventually
        _climateSensors["interior"] = std::make_unique<hw::DHTxx>(hw::DHTxx::Type::DHT22, io::GPIO::claim(14));
        _climateSensors["exterior"] = std::make_unique<hw::DHTxx>(hw::DHTxx::Type::DHT22, io::GPIO::claim(16));
    }

    //==============================================================================
    // Build set of arguments
    static const std::set<Argument>& getKnownArgs()
    {
        static std::set<Argument> knownArgs = {
            Argument {
                "rest-port",
                "Port to use for REST API listener (default: 8080)",
                "REST port",
                'r'
            },

            Argument {
                "db-host",
                "MongoDB host (default: 127.0.0.1)",
                "ip/hostname",
                'D'
            },

            Argument {
                "db-port",
                "Port to use to connect to MongoDB (default: 5432)",
                "DB port",
                'd'
            },

            Argument {
                "db-name",
                "MongoDB database name (default: beewatch)",
                "db name"
            },

            Argument {
                "debug",
                "Enable debug logging (default: off)",
                {}
            },

            Argument {
                "help",
                "Display this help message",
                {},
                'h'
            }
        };

        return knownArgs;
    }

    void Manager::init(int argc, char * argv[])
    {
        setName(argv[0]);

        // Defaults
        int restPort = http::Server::DEFAULT_PORT;

        std::string dbName = DB::DEFAULT_NAME;
        std::string dbHost = DB::DEFAULT_HOST;
        int dbPort = DB::DEFAULT_PORT;

        // Parse args
        const auto& knownArgs = getKnownArgs();

        for (int i = 1; i < argc; ++i)
        {
            std::string arg = argv[i];

            auto match = std::find(knownArgs.begin(), knownArgs.end(), arg);

            // TODO: It would be nice to wrap the arg parsing in a class
            if (match == knownArgs.end())
            {
                std::cerr << "Received unknown argument: \"" << arg << "\"" << std::endl;
                printUsage();
                exit(-1);
            }
            else if (*match == "--rest-port")
            {
                if (i+1 < argc && argv[i+1][0] != '-')
                {
                    try
                    {
                        restPort = std::stoi(argv[++i]);
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "Received invalid option for \"" << arg << "\": \""
                                  << argv[i] << "\"" << std::endl;

                        printUsage();
                        exit(-1);
                    }
                }
                else
                {
                    std::cerr << "Expected " << match->expectedArg << " after \"" << arg << "\"" << std::endl;
                    printUsage();
                    exit(-1);
                }
            }
            else if (*match == "--db-host")
            {
                if (i+1 < argc && argv[i+1][0] != '-')
                {
                    dbHost = argv[++i];
                }
                else
                {
                    std::cerr << "Expected " << match->expectedArg << " after \"" << arg << "\"" << std::endl;
                    printUsage();
                    exit(-1);
                }
            }
            else if (*match == "--db-port")
            {
                if (i+1 < argc && argv[i+1][0] != '-')
                {
                    try
                    {
                        dbPort = std::stoi(argv[++i]);
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "Received invalid option for \"" << arg << "\": \""
                                  << argv[i] << "\"" << std::endl;

                        printUsage();
                        exit(-1);
                    }
                }
                else
                {
                    std::cerr << "Expected " << match->expectedArg << " after \"" << arg << "\"" << std::endl;
                    printUsage();
                    exit(-1);
                }
            }
            else if (*match == "--db-name")
            {
                if (i+1 < argc && argv[i+1][0] != '-')
                {
                    dbName = argv[++i];
                }
                else
                {
                    std::cerr << "Expected " << match->expectedArg << " after \"" << arg << "\"" << std::endl;
                    printUsage();
                    exit(-1);
                }
            }
            else if (*match == "--debug")
            {
                g_logger.info("Enabling debug logs");
                g_logger.setVerbosity(Logger::Level::Debug);
            }
            else if (*match == "--help")
            {
                printUsage();
                exit(0);
            }
        }

        try
        {
            _apiServer = std::make_unique<http::Server>(*this, restPort);
        }
        catch (const std::exception& e)
        {
            g_logger.fatal("Caught exception while bringing up API server: " +
                           std::string(e.what()));

            exit(-1);
        }

        try
        {
            _db = std::make_unique<DB>(dbName, dbHost, dbPort);
        }
        catch (const std::exception& e)
        {
            g_logger.fatal("Caught exception while bringing up DB connection: " +
                           std::string(e.what()));

            exit(-1);
        }
    }

    void Manager::printUsage()
    {
        const auto& knownArgs = getKnownArgs();

        // First line:
        //  exe [-1, --long-1 "required argument"] [-2, --long-2] ...
        std::cout << getName();

        for (const auto& arg : knownArgs)
        {
            std::cout << " [" << arg.helpShort() << "]";
        }

        std::cout << std::endl;

        // Following lines:
        //      -1, --long-1 "required argument"    Argument description message
        for (const auto& arg : knownArgs)
        {
            std::cout << "    " << arg.helpLong() << std::endl;
        }
    }

    //==============================================================================
    void Manager::start(bool blocking)
    {
        _ctrlThread = std::make_unique<std::thread>(&Manager::ctrlLoop, this);

        if (blocking)
        {
            _ctrlThread->join();
        }
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

        try
        {

        }
        catch (const std::runtime_error& e)
        {
            g_logger.error("Failed to update name in DB: " + std::string(e.what()));
        }

        _name = name;
    }

    //==============================================================================
    std::map<int64_t, ClimateData<double>> Manager::getClimateSamples(std::string sensorID,
                                                                      int64_t since) const
    {
        return _db->getClimateData(sensorID, since);
    }

    void Manager::clearClimateData()
    {
        _db->clearClimateData();
    }

    std::vector<std::string> Manager::getClimateSensorIDs() const
    {
        std::vector<std::string> sensorIDs;

        for (auto& climateSensor: _climateSensors)
        {
            sensorIDs.push_back(climateSensor.first);
        }

        return sensorIDs;
    }
    

    //==============================================================================
    void Manager::ctrlLoop()
    {
        while (1)
        {
            auto start = std::chrono::steady_clock::now();

            // TODO: Read data on both internal & external sensors
            for (auto& climateSensor: _climateSensors)
            {
                auto& id = climateSensor.first;
                auto& sensor = climateSensor.second;

                auto data = sensor->read();

                g_logger.debug("Humidity (" + id + "): " + string::fromNumber(data.humidity) + " %");
                g_logger.debug("Temperature (" + id + "): " + string::fromNumber(data.temperature) + " deg Celsius");

                if (data.temperature < 60.0 && data.humidity <= 100.0)
                {
                _db->addClimateData(id, g_timeReal.toUnix(g_timeReal.now()), data);
                }
                else
                {
                    g_logger.warning("DHTxx read failed on sensor " + id);
                }
            }

            // Measure mass
            //auto mass = hx711->read();
            //
            //g_logger.debug("Mass: " + numToStr(mass) + " kg");

            std::this_thread::sleep_until(start + std::chrono::minutes(5));
        }
    }

} // namespace beewatch
