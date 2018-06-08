//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "util/logging.h"

#include <ctime>
#include <iomanip>
#include <sstream>


namespace beewatch
{

    static std::string getTimeLocal()
    {
        std::time_t now = std::time(nullptr);
        std::tm nowLocal = *std::localtime(&now);

        std::ostringstream oss;
        oss << std::put_time(&nowLocal, "%Y-%m-%d %H:%M:%S");

        return oss.str();
    }

    //==============================================================================
    Logger::Logger()
        : _verbosity((Logger::Level)LOG_WARNING)
    {
        // Open log file
        openlog(BEEWATCH_PROJECT_NAME, LOG_CONS | LOG_NDELAY, LOG_USER);
        setlogmask(LOG_UPTO(LOG_DEBUG));

        // Write header
        std::string header = "Beginning log: " BEEWATCH_NAME_VERSION;

        dualPrint(Info, header);

        setlogmask(LOG_UPTO(LOG_WARNING));
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
    void Logger::print(Level logLevel, const std::string& msg)
    {
        if (logLevel > _verbosity)
        {
            return;
        }

        // Format message with timestamp and severity
        std::string msgFormatted;

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

        msgFormatted += msg;

        // Print formatted message to log file (thread-safe)
        syslog((int)logLevel, "%s", msg.c_str());
    }

    void Logger::dualPrint(Level logLevel, const std::string& msg)
    {
        // TODO: Print to somewhere in UI as well so user knows about these errors
        std::cerr << msg;
        print(logLevel, msg);
    }


    //==============================================================================
    /**
    * Global logger object
    */
    Logger& logger = Logger::getInstance();

} // namespace beewatch
