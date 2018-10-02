//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "timing.h"

#include <chrono>
#include <thread>

#ifdef WIN32
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
    Time::Time()
    {
        // Initialise start time
        clock_gettime(CLOCK_MONOTONIC_RAW, &_start);
    }

    //==============================================================================
    Time& Time::getInstance()
    {
        static Time timeSingleton;
        return timeSingleton;
    }

    //==============================================================================
    double Time::now() const
    {
        timespec now;
        clock_gettime(CLOCK_MONOTONIC_RAW, &now);

        return ((now.tv_sec - _start.tv_sec) * 1e3) + ((now.tv_nsec - _start.tv_nsec) / 1e6);
    }

    //==============================================================================
    void Time::wait(double timeMs) const
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

    //==============================================================================
    Time& g_time = Time::getInstance();

} // namespace beewatch
