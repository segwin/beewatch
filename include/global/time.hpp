//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "util/patterns.hpp"

#include <chrono>
#include <cmath>
#include <ctime>
#include <thread>

#ifdef WIN32
#define CLOCK_REALTIME      0
#define CLOCK_MONOTONIC_RAW 4

// Windows clock_gettime() adapted from Asain Kujovic's implementation:
// https://stackoverflow.com/a/31335254
#include <Windows.h>
#include <cstdint>

#define exp7           10000000i64     //1E+7
#define exp9         1000000000i64     //1E+9
#define w2ux 116444736000000000i64     //1.jan1601 to 1.jan1970

static void unix_time(struct timespec *spec)
{
    int64_t wintime;

    GetSystemTimeAsFileTime((FILETIME*)&wintime);
    wintime -= w2ux;

    spec->tv_sec = wintime / exp7;
    spec->tv_nsec = wintime % exp7 * 100;
}

int clock_gettime(int, timespec *spec)
{
    static timespec start;
    static int64_t startTicks;
    static int64_t ticksPerS = 0;
    static double nsPerTick = 0.0;

    if (!ticksPerS)
    {
        // Init static values once
        unix_time(&start);
        QueryPerformanceCounter((LARGE_INTEGER*)&startTicks);

        QueryPerformanceFrequency((LARGE_INTEGER*)&ticksPerS);
        nsPerTick = (double)exp9 / (double)ticksPerS;
    }

    int64_t nowTicks;

    QueryPerformanceCounter((LARGE_INTEGER*)&nowTicks);
    nowTicks -= startTicks;

    spec->tv_sec = start.tv_sec + (nowTicks / ticksPerS);
    spec->tv_nsec = start.tv_nsec + (decltype(timespec::tv_nsec))((nowTicks % ticksPerS) * nsPerTick);

    if (spec->tv_nsec >= exp9)
    {
        spec->tv_sec++;
        spec->tv_nsec -= exp9;
    }

    return 0;
}
#endif

namespace beewatch
{
    //==============================================================================
    /**
     * @class Time
     *
     * Keeps track of current time and
     */
    template <int CLOCK>
    class Time : public singleton_t<Time<CLOCK>>
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
        double now() const
        {
            timespec now;
            clock_gettime(CLOCK, &now);

            return ((now.tv_sec - _start.tv_sec) * 1e3) + ((now.tv_nsec - _start.tv_nsec) / 1e6);
        }

        //==============================================================================
        /**
         * @brief Convert Time's timestamp to Unix time
         *
         * @param Timestamp returned by Time::now()
         *
         * @returns Equivalent Unix timestamp
         */
        int64_t toUnix(double timeMs) const
        {
            double startTimeS = _start.tv_sec + 1e9 * _start.tv_nsec;
            return (int64_t)std::round((timeMs / 1000.0) + startTimeS);
        }

        //==============================================================================
        /**
         * @brief Wait for ms milliseconds (note: uses active wait loop if <100us)
         *
         * @param [in] ms       Time to wait (in ms)
         */
        void wait(double timeMs) const
        {
            // If under 1ms: use active wait
            if (timeMs < 1.0)
            {
                double startMs = now();
                while (now() < startMs + timeMs) {}

                return;
            }

            // Otherwise: use std::this_thread::sleep_for()
            using namespace std::chrono;

            if (timeMs >= 1)
                std::this_thread::sleep_for( std::chrono::microseconds((size_t)(timeMs * 1e3)) );
            else if (timeMs >= 1e-3)
                std::this_thread::sleep_for( std::chrono::nanoseconds((size_t)(timeMs * 1e6)) );
        }


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
        Time()
        {
            // Initialise start time
            clock_gettime(CLOCK, &_start);
        }


    private:
        //==============================================================================
        /// Time point at construction
        timespec _start;
    };

    //==============================================================================
    #define g_timeReal  Time<CLOCK_REALTIME>::get()
    #define g_timeRaw   Time<CLOCK_MONOTONIC_RAW>::get()
}
