//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/gpio.h"
#include "algorithms.h"

#include "catch.hpp"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

//==============================================================================
SCENARIO("GPIOs can be claimed and released", "[gpio]")
{
    GIVEN("all GPIOs are initially free")
    {
        REQUIRE(io::GPIO::getAvailableIDs().size() == io::GPIO::NUM_GPIO);

        WHEN("we claim a valid GPIO")
        {
            io::GPIO::Ptr gpio = io::GPIO::claim(4);

            THEN("we receive a unique pointer to that GPIO with the default config")
            {
                REQUIRE(gpio != nullptr);
                REQUIRE(gpio->getID() == 4);
                REQUIRE(gpio->getMode() == io::GPIO::Mode::Input);
            }

            THEN("that GPIO can no longer be claimed")
            {
                io::GPIO::Ptr sameGPIO = io::GPIO::claim(4);
                REQUIRE(sameGPIO == nullptr);
            }

            AND_WHEN("we set the GPIO's mode, then release it")
            {
                gpio->setMode(io::GPIO::Mode::Output);

                REQUIRE(gpio->getMode() == io::GPIO::Mode::Output);

                gpio.reset();

                THEN("all GPIOs are free again, including the released GPIO")
                {
                    std::vector<int> availableIDs = io::GPIO::getAvailableIDs();

                    REQUIRE(availableIDs.size() == io::GPIO::NUM_GPIO);
                    REQUIRE(contains(availableIDs.begin(), availableIDs.end(), 4));
                }
                
                THEN("it can be claimed again and has the default config")
                {
                    io::GPIO::Ptr sameGPIO = io::GPIO::claim(4);

                    REQUIRE(sameGPIO != nullptr);
                    REQUIRE(sameGPIO->getID() == 4);
                    REQUIRE(sameGPIO->getMode() == io::GPIO::Mode::Input);
                }
            }
        }

        WHEN("we try to claim a GPIO with a negative ID")
        {
            bool caughtRangeErr = false;

            try
            {
                io::GPIO::Ptr gpio = io::GPIO::claim(-1);
            }
            catch (std::range_error)
            {
                caughtRangeErr = true;
            }

            THEN("a range error is thrown")
            {
                REQUIRE(caughtRangeErr);
            }
        }

        WHEN("we try to claim a GPIO with non-existent ID")
        {
            bool caughtRangeErr = false;

            try
            {
                io::GPIO::Ptr gpio = io::GPIO::claim(io::GPIO::NUM_GPIO);
            }
            catch (std::range_error)
            {
                caughtRangeErr = true;
            }

            THEN("a range error is thrown")
            {
                REQUIRE(caughtRangeErr);
            }
        }
    }

    GIVEN("all GPIOs are already claimed")
    {
        std::vector<io::GPIO::Ptr> claimedGPIOs;

        for (int i = 0; i < io::GPIO::NUM_GPIO; ++i)
        {
            claimedGPIOs.push_back(io::GPIO::claim(i));
        }

        REQUIRE(io::GPIO::getAvailableIDs().empty());

        WHEN("we check their IDs")
        {
            THEN("all GPIOs have the expected ID values")
            {
                REQUIRE(claimedGPIOs.size() == io::GPIO::NUM_GPIO);

                for (int i = 0; i < io::GPIO::NUM_GPIO; ++i)
                {
                    REQUIRE(claimedGPIOs[i]->getID() == i);
                }
            }
        }

        WHEN("we try to claim any GPIO")
        {
            io::GPIO::Ptr gpio = io::GPIO::claim(12);

            THEN("we receive nullptr")
            {
                REQUIRE(gpio == nullptr);
            }
        }

        WHEN("one is released")
        {
            claimedGPIOs[12].reset();

            THEN("it becomes available for us to claim")
            {
                io::GPIO::Ptr gpio = io::GPIO::claim(12);

                REQUIRE(gpio != nullptr);
                REQUIRE(gpio->getID() == 12);
            }
        }
    }
}
