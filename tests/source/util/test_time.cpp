//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "global/time.hpp"
#include "util/priority.h"

#include "catch.hpp"

#include <chrono>
#include <thread>

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;
using namespace std::chrono;

//==============================================================================
SCENARIO("All Time objects with the same clock should refer to the same instance", "[time][util][core]")
{
    GIVEN("the global Time instances")
    {
        WHEN("we get a new reference to the monotonic Time instance")
        {
            auto& time2 = Time<CLOCK_MONOTONIC_RAW>::get();

            THEN("the two references should refer to the same object")
            {
                REQUIRE(&g_timeRaw == &time2);
            }
        }

        WHEN("we get a new reference to the realtime Time instance")
        {
            auto& time2 = Time<CLOCK_REALTIME>::get();

            THEN("the two references should refer to the same object")
            {
                REQUIRE(&g_timeReal == &time2);
            }
        }
    }
}

//==============================================================================
SCENARIO("Use Time::wait() to delay by a given amount of time", "[time][util][timingSensitive][!mayfail]")
{
    GIVEN("the global monotonic Time instance and a high process priority")
    {
        PriorityGuard priority(Priority::RealTime);

        WHEN("we call time.wait() for 1s")
        {
            auto start = high_resolution_clock::now();
            g_timeRaw.wait(1e3);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 1s +/- 5ms")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();
                REQUIRE(diffUs == Approx(1e6).margin(5e3));
            }
        }

        WHEN("we call time.wait() for 10ms")
        {
            auto start = high_resolution_clock::now();
            g_timeRaw.wait(10.0);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 10ms +/- 1ms")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();
                REQUIRE(diffUs == Approx(10e3).margin(1e3));
            }
        }

        WHEN("we call time.wait() for 1ms")
        {
            auto start = high_resolution_clock::now();
            g_timeRaw.wait(1.0);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 1ms +/- 500us")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();
                REQUIRE(diffUs == Approx(1e3).margin(500.0));
            }
        }

        WHEN("we call time.wait() for 50us")
        {
            auto start = high_resolution_clock::now();
            g_timeRaw.wait(50e-3);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 50us +/- 5us")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();
                REQUIRE(diffUs == Approx(50.0).margin(5.0));
            }
        }

        WHEN("we call time.wait() for 10us")
        {
            auto start = high_resolution_clock::now();
            g_timeRaw.wait(10e-3);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 10us +/- 2.5us")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();
                REQUIRE(diffUs == Approx(10.0).margin(5.0));
            }
        }

        WHEN("we call time.wait() for 1us")
        {
            auto start = high_resolution_clock::now();
            g_timeRaw.wait(1e-3);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 1us +/- 3us")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();
                REQUIRE(diffUs == Approx(1.0).margin(3.0));
            }
        }
    }
}

//==============================================================================
SCENARIO("Use Time::now() to get current time", "[time][util][timingSensitive][!mayfail]")
{
    GIVEN("the global monotonic Time instance and a high process priority")
    {
        PriorityGuard priority(Priority::RealTime);

        WHEN("we call time.now()")
        {
            double now = g_timeRaw.now();

            THEN("we get some arbritrary non-negative value")
            {
                REQUIRE(now >= 0.0);
            }
        }

        WHEN("we call time.now() twice with a 1s interval")
        {
            double startMs = g_timeRaw.now();
            g_timeRaw.wait(1e3);
            double endMs = g_timeRaw.now();

            THEN("the difference should be 1s +/- 1ms")
            {
                double diffMs = endMs - startMs;
                REQUIRE(diffMs == Approx(1e3).margin(1.0));
            }
        }

        WHEN("we call time.now() twice with a 10ms interval")
        {
            double startMs = g_timeRaw.now();
            g_timeRaw.wait(10.0);
            double endMs = g_timeRaw.now();

            THEN("the difference should be 10ms +/- 500us")
            {
                double diffMs = endMs - startMs;
                REQUIRE(diffMs == Approx(10.0).margin(1.0));
            }
        }

        WHEN("we call time.now() twice with a 1ms interval")
        {
            double startMs = g_timeRaw.now();
            g_timeRaw.wait(1.0);
            double endMs = g_timeRaw.now();

            THEN("the difference should be 1ms +/- 100us")
            {
                double diffMs = endMs - startMs;
                REQUIRE(diffMs == Approx(1.0).margin(0.5));
            }
        }

        WHEN("we call time.now() twice with a 100us interval")
        {
            double startMs = g_timeRaw.now();
            g_timeRaw.wait(100e-3);
            double endMs = g_timeRaw.now();

            THEN("the difference should be 100us +/- 5us")
            {
                double diffMs = endMs - startMs;
                REQUIRE(diffMs == Approx(100e-3).margin(5e-3));
            }
        }

        WHEN("we call time.now() twice with a 10us interval")
        {
            double startMs = g_timeRaw.now();
            g_timeRaw.wait(10e-3);
            double endMs = g_timeRaw.now();

            THEN("the difference should be 10us +/- 5us")
            {
                double diffMs = endMs - startMs;
                REQUIRE(diffMs == Approx(10e-3).margin(5e-3));
            }
        }
    }

    // TODO: Add tests for g_timeReal object (compare to system time)
}
