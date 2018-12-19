//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "global/logging.h"

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
        { Logger::Level::Unattainable, LOG_EMERG },
        { Logger::Level::Fatal,        LOG_EMERG },
        { Logger::Level::Error,        LOG_ERR },
        { Logger::Level::Warning,      LOG_WARNING },
        { Logger::Level::Notice,       LOG_NOTICE },
        { Logger::Level::Info,         LOG_INFO },
        { Logger::Level::Debug,        LOG_DEBUG },
    };

    //==============================================================================
    Logger::Logger()
    {
        // Open log file
        openlog(PROJECT_NAME, LOG_CONS | LOG_NDELAY, LOG_LOCAL4);
        setVerbosity(Level::Info);

        // Write header
        std::string divider = "============================================================";
        std::string announcement = "Beginning log: " NAME_VERSION;

        log(Level::Info, "");
        log(Level::Info, divider);
        log(Level::Info, announcement);
        log(Level::Info, divider);
        log(Level::Info, "");

        setVerbosity(Level::Warning);
    }

    Logger::~Logger()
    {
        closelog();
    }

    //==============================================================================
    void Logger::setVerbosity(Level logLevel)
    {
        _verbosity = logLevel;
        setlogmask( LOG_UPTO(mapLevelToSyslog.at(logLevel)) );
    }

    //==============================================================================
    void Logger::print(Level logLevel, std::string msg, bool addLevel)
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
            case Level::Fatal:
                msgFormatted = "[FATAL] ";
                break;

            case Level::Error:
                msgFormatted = "[ERROR] ";
                break;

            case Level::Warning:
                msgFormatted = "[WARNING] ";
                break;

            case Level::Notice:
                msgFormatted = "[NOTICE] ";
                break;

            case Level::Info:
                msgFormatted = "[INFO] ";
                break;

            case Level::Debug:
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
    
    //==============================================================================
    void Logger::fatal(std::string msg) { print(Level::Fatal, msg); }
    void Logger::error(std::string msg) { print(Level::Error, msg); }
    void Logger::warning(std::string msg) { print(Level::Warning, msg); }
    void Logger::notice(std::string msg) { print(Level::Notice, msg); }
    void Logger::info(std::string msg) { print(Level::Info, msg); }
    void Logger::debug(std::string msg) { print(Level::Debug, msg); }

} // namespace beewatch
