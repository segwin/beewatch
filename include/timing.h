//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "patterns.h"

#include <ctime>

namespace beewatch
{
    //==============================================================================
    /**
     * @class Time
     *
     * Keeps track of current time and 
     */
    class Time : public singleton_t<Time>
    {
    public:
        //==============================================================================
        ~Time() = default;

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


    protected:
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


    private:
        //==============================================================================
        /// Time point at construction
        timespec _start;
    };

    //==============================================================================
    extern Time& g_time;
}
