//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "logging.h"

#include "version.h"

#include <iostream>
#include <map>
#include <string>

#ifdef __linux__
#include <syslog.h>
#else
// Mock syslog definitions: not intended to accurately reflect actual values
#define LOG_EMERG   0
#define LOG_ERR     1
#define LOG_WARNING 2
#define LOG_NOTICE  3
#define LOG_INFO    4
#define LOG_DEBUG   5

#define LOG_CONS    0
#define LOG_NDELAY  1
#define LOG_USER    2

#define LOG_UPTO(x) x

void openlog(const char *, int, int) {}
void syslog(int, const char *, ...) {}
void closelog(void) {}

int setlogmask(int) { return 0; }
#endif

namespace beewatch
{

    //==============================================================================
    const std::map<Logger::Level, int> mapLevelToSyslog = {
        { Logger::Fatal,    LOG_EMERG },
        { Logger::Error,    LOG_ERR },
        { Logger::Warning,  LOG_WARNING },
        { Logger::Notice,   LOG_NOTICE },
        { Logger::Info,     LOG_INFO },
        { Logger::Debug,    LOG_DEBUG },
    };

    //==============================================================================
    Logger::Logger()
    {
        // Open log file
        openlog(PROJECT_NAME, LOG_CONS | LOG_NDELAY, LOG_USER);
        setVerbosity(Info);

        // Write header
        std::string divider = "============================================================";
        std::string announcement = "Beginning log: " NAME_VERSION;

        print(Info, "", false);
        print(Info, divider, false);
        print(Info, announcement, false);
        print(Info, divider, false);
        print(Info, "", false);

        setlogmask(mapLevelToSyslog.at(Warning));
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
        setlogmask( LOG_UPTO(mapLevelToSyslog.at(logLevel)) );
    }


    //==============================================================================
    void Logger::print(Level logLevel, const std::string& msg, bool addLevel)
    {
        if (logLevel > _verbosity)
        {
            return;
        }

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

        // TODO: Print important messages to somewhere in UI so user knows about them
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
        syslog(mapLevelToSyslog.at(logLevel), "%s", msg.c_str());
    }

} // namespace beewatch
