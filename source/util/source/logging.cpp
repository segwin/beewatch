//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "util/logging.h"

#include <ctime>
#include <iomanip>
#include <sstream>


namespace beewatch
{

    //==============================================================================
    Logger::Logger()
    {
        // Open log file
        openlog(BEEWATCH_PROJECT_NAME, LOG_CONS | LOG_NDELAY, LOG_USER);
        setVerbosity(Debug);

        // Write header
        std::string divider = "============================================================";
        std::string announcement = "Beginning log: " BEEWATCH_NAME_VERSION;

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
        static Logger logger;
        return logger;
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

        // TODO: Print to somewhere in UI as well so user knows about these errors
        std::cerr << msgFormatted << std::endl;
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
