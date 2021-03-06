//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "util/patterns.hpp"

#include <string>

namespace beewatch
{

    //==============================================================================
    /**
     * @class Logger
     *
     * Offers methods to log a message to the error console and to disk.
     * Filters messages by severity based on the chosen verbosity level.
     */
    class Logger : public singleton_t<Logger>
    {
    public:
        //==============================================================================
        ~Logger();


        //==============================================================================
        enum class Level
        {
            Unattainable = -1,
            Fatal = 0,
            Error = 1,
            Warning = 2,
            Notice = 3,
            Info = 4,
            Debug = 5,
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
        void print(Level logLevel, std::string msg, bool addLevel = true);


        //==============================================================================
        /// Print message with Fatal level
        void fatal(std::string msg);

        /// Print message with Error level
        void error(std::string msg);

        /// Print message with Warning level
        void warning(std::string msg);

        /// Print message with Notice level
        void notice(std::string msg);

        /// Print message with Info level
        void info(std::string msg);

        /// Print message with Debug level
        void debug(std::string msg);

    
    protected:
        //==============================================================================
        /**
         * @brief Construct a Logger object
         *
         * Opens log file in the system-appropriate log directory, limiting its size if
         * necessary. 
         *
         * NB: Class is intended to be used as a singleton. Use Logger::getInstance() to
         * get a reference to the object.
         */
        Logger();


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
        Level _verbosity;
    };

    #define g_logger    Logger::get()

} // namespace beewatch
