//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <string>

namespace beewatch
{

    //================================================================
    /**
     * @class Logger
     *
     * Offers methods to log a message to the error console and to disk.
     * Filters messages by severity based on the chosen verbosity level.
     */
    class Logger
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
            Fatal,
            Error,
            Warning,
            Notice,
            Info,
            Debug,
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
        * @brief Print message to console/UI and to log file
        *
        * All messages get printed to the console. Logged messages are filtered by
        * severity depending on current verbosity setting (see Logger::print()).
        *
        * @param [in] logLevel  Severity of message to print
        * @param [in] msg       Message to print
        * @param [in] addLevel  If true, prepend message with severity level
        */
        void print(Level logLevel, const std::string& msg, bool addLevel = true);


    private:
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
        void log(Level logLevel, const std::string& msg);


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

    #define logger Logger::getInstance()

} // namespace beewatch
