//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <util/util_api.h>

#include <beewatch/beewatch-version.h>

#include <iostream>
#include <fstream>
#include <memory>
#include <mutex>
#include <string>

#include <syslog.h>


namespace beewatch
{

    //================================================================
    /**
     * @class Logger
     *
     * Offers methods to log a message to the error console and to disk.
     * Filters messages by severity based on the chosen verbosity level.
     */
    class UTIL_API Logger
    {
    public:
        //==============================================================================
        Logger(const Logger&) = delete;
        void operator=(const Logger&) = delete;

        ~Logger();


        //==============================================================================
        /**
         * @brief Returns a reference to the singleton class instance
         *
         * @returns Singleton Logger object
         */
        static Logger& getInstance();


        //==============================================================================
        enum Level
        {
            Fatal = LOG_EMERG,
            Error = LOG_ERR,
            Warning = LOG_WARNING,
            Notice = LOG_NOTICE,
            Info = LOG_INFO,
            Debug = LOG_DEBUG
        };

        /**
        * @brief Set logger verbosity level
        *
        * Determines how severe a message must be before being printed. When verbosity is
        * low, only very important messages are logged.
        *
        * @param [in] logLevel New verbosity level
        */
        void setVerbosity(Level logLevel);


        //==============================================================================
        /**
        * @brief Print message to log file
        *
        * If current verbosity level permits it, message is formatted and printed
        * to app log file.
        *
        * @param [in] logLevel Severity of message to print
        * @param [in] msg      Message to print
        */
        void print(Level logLevel, const std::string& msg);

        /**
        * @brief Print message to console/UI and to log file
        *
        * All messages get printed to the console. Logged messages are filtered by
        * severity depending on current verbosity setting (see Logger::print()).
        *
        * @param [in] logLevel Severity of message to print
        * @param [in] msg      Message to print
        */
        void dualPrint(Level logLevel, const std::string& msg);


    private:
        //==============================================================================
        /**
        * @brief Construct a logger object
        *
        * Opens log file in the system-appropriate log directory, limiting its size if
        * necessary. 
        *
        * NB: Class is intended to be used as a singleton. Use Logger::getInstance() to
        * get a reference to the object.
        */
        Logger();


        //==============================================================================
        Level _verbosity;
    };

    extern UTIL_API Logger& logger;

} // namespace beewatch
