//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include <ctime>

#ifdef WIN32
#define CLOCK_MONOTONIC_RAW 4

int clock_gettime(int, timespec *spec);
#endif

namespace beewatch
{
    //==============================================================================
    /**
     * @class Time
     *
     * Keeps track of current time and 
     */
    class Time
    {
    public:
        //==============================================================================
        Time(const Time&) = delete;
        void operator=(const Time&) = delete;

        ~Time() = default;

        //==============================================================================
        /**
         * @brief Returns a reference to the singleton class instance
         *
         * @returns Singleton Time object
         */
        static Time& getInstance();

        //==============================================================================
        /**
         * @brief Get milliseconds since beginning of Time
         *
         * @returns Number of milliseconds since object construction
         */
        double now() const;

        //==============================================================================
        /**
         * @brief Wait for ms milliseconds (note: uses active wait loop if <100us)
         * 
         * @param [in] ms       Time to wait (in ms)
         */
        void wait(double timeMs) const;

    private:
        //==============================================================================
        /**
         * @brief Construct a Time object
         *
         * Opens log file in the system-appropriate log directory, limiting its size if
         * necessary.
         *
         * NB: Class is intended to be used as a singleton. Use Logger::getInstance() to
         * get a reference to the object.
         */
        Time();

        //==============================================================================
        /// Time point at construction
        timespec _start;
    };

    //==============================================================================
    extern Time& g_time;
}