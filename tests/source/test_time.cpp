//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "timing.h"

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
SCENARIO("A Time object can be used to wait & get current time", "[util][!mayfail]")
{
    GIVEN("the global Time instance")
    {
        WHEN("we get a new reference to the Time instance")
        {
            Time& time2 = Time::getInstance();

            THEN("the two references should refer to the same object")
            {
                REQUIRE(&g_time == &time2);
            }
        }

        //==============================================================================
        // Time::now()
        WHEN("we call time.now()")
        {
            double now = g_time.now();

            THEN("we get some arbritrary non-negative value")
            {
                REQUIRE(now >= 0.0);
            }
        }

        WHEN("we call time.now() twice with a 1s interval")
        {
            double startMs = g_time.now();
            std::this_thread::sleep_until(high_resolution_clock::now() + seconds(1));
            double endMs = g_time.now();

            THEN("the difference should be 1000ms +/- 5ms")
            {
                double diffMs = endMs - startMs;

                REQUIRE(diffMs > 0.995 * 1000.0);
                REQUIRE(diffMs < 1.005 * 1000.0);
            }
        }

        WHEN("we call time.now() twice with a 10ms interval")
        {
            double startMs = g_time.now();
            std::this_thread::sleep_until(high_resolution_clock::now() + milliseconds(10));
            double endMs = g_time.now();

            THEN("the difference should be 10ms +/- 1ms")
            {
                double diffMs = endMs - startMs;

                REQUIRE(diffMs > 10.0 - 1.0);
                REQUIRE(diffMs < 10.0 + 1.0);
            }
        }

        WHEN("we call time.now() twice with a 1ms interval")
        {
            double startMs = g_time.now();
            std::this_thread::sleep_until(high_resolution_clock::now() + milliseconds(1));
            double endMs = g_time.now();

            THEN("the difference should be 1ms +/- 1ms")
            {
                double diffMs = endMs - startMs;

                REQUIRE(diffMs > 1.0 - 1.0);
                REQUIRE(diffMs < 1.0 + 1.0);
            }
        }

        //==============================================================================
        // Time::wait()
        WHEN("we call time.wait() for 1s")
        {
            auto start = high_resolution_clock::now();
            g_time.wait(1e3);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 1s +/- 5ms")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();

                REQUIRE(diffUs > 1e6 - 5e3);
                REQUIRE(diffUs < 1e6 + 5e3);
            }
        }

        WHEN("we call time.wait() for 10ms")
        {
            auto start = high_resolution_clock::now();
            g_time.wait(10.0);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 10ms +/- 1ms")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();

                REQUIRE(diffUs > 10e3 - 1e3);
                REQUIRE(diffUs < 10e3 + 1e3);
            }
        }

        WHEN("we call time.wait() for 1ms")
        {
            auto start = high_resolution_clock::now();
            g_time.wait(1.0);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 1ms +/- 500us")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();

                REQUIRE(diffUs > 1e3 - 500.0);
                REQUIRE(diffUs < 1e3 + 500.0);
            }
        }

        WHEN("we call time.wait() for 50us")
        {
            auto start = high_resolution_clock::now();
            g_time.wait(50e-3);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 50us +/- 2.5us")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();

                REQUIRE(diffUs > 50.0 - 2.5);
                REQUIRE(diffUs < 50.0 + 2.5);
            }
        }

        WHEN("we call time.wait() for 10us")
        {
            auto start = high_resolution_clock::now();
            g_time.wait(10e-3);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 10us +/- 1us")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();

                REQUIRE(diffUs > 10.0 - 1.0);
                REQUIRE(diffUs < 10.0 + 1.0);
            }
        }

        WHEN("we call time.wait() for 1us")
        {
            auto start = high_resolution_clock::now();
            g_time.wait(1e-3);
            auto end = high_resolution_clock::now();

            THEN("we should wait for 1us +/- 500ns")
            {
                double diffUs = (double)duration_cast<microseconds>(end - start).count();

                REQUIRE(diffUs > 1.0 - 0.5);
                REQUIRE(diffUs < 1.0 + 0.5);
            }
        }
    }
}