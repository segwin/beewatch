//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "logging.h"

#include "version.h"

#include <iostream>
#include <string>

namespace beewatch
{

    //==============================================================================
    Logger::Logger()
    {
        // Open log file
        openlog(PROJECT_NAME, LOG_CONS | LOG_NDELAY, LOG_USER);
        setVerbosity(Debug);

        // Write header
        std::string divider = "============================================================";
        std::string announcement = "Beginning log: " NAME_VERSION;

        print(Debug, "", false);
        print(Debug, divider, false);
        print(Debug, announcement, false);
        print(Debug, divider, false);
        print(Debug, "", false);

        setlogmask(Warning);
    }

    Logger::~Logger()
    {
        closelog();
    }


    //==============================================================================
    Logger& Logger::getInstance()
    {
        static Logger loggerSingleton;
        return loggerSingleton;
    }


    //==============================================================================
    void Logger::setVerbosity(Level logLevel)
    {
        _verbosity = logLevel;
        setlogmask(LOG_UPTO((int)logLevel));
    }


    //==============================================================================
    void Logger::print(Level logLevel, const std::string& msg, bool addLevel)
    {
        // Format message with timestamp and severity
        std::string msgFormatted;

        if (addLevel)
        {
            switch (logLevel)
            {
            case Fatal:
                msgFormatted = "[FATAL] ";
                break;

            case Error:
                msgFormatted = "[ERROR] ";
                break;

            case Warning:
                msgFormatted = "[WARNING] ";
                break;

            case Notice:
                msgFormatted = "[NOTICE] ";
                break;

            case Info:
                msgFormatted = "[INFO] ";
                break;

            case Debug:
            default:
                msgFormatted = "[DEBUG] ";
                break;
            }
        }

        msgFormatted += msg;

        // TODO: Print important messages to somewhere in UI as well so user knows about them
        std::cout << msgFormatted << std::endl;
        log(logLevel, msgFormatted);
    }

    void Logger::log(Level logLevel, const std::string& msg)
    {
        if (logLevel > _verbosity)
        {
            return;
        }

        // Print formatted message to log file (thread-safe)
        syslog((int)logLevel, "%s", msg.c_str());
    }


    //==============================================================================
    /**
    * Global logger object
    */
    Logger& logger = Logger::getInstance();

} // namespace beewatch
