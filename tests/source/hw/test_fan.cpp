//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "hw/fan.h"
#include "logging.h"

#include "catch.hpp"

#include "gpioMock.hpp"
#include "pwmMock.hpp"
#include "tachometerMock.hpp"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

using hw::Fan;
using io::PWM;
using io::Tachometer;

//==============================================================================
SCENARIO("A Fan object can be built for a 3-pin fan (PWM only)")
{
    // Disable logger
    g_logger.setVerbosity(Logger::Unattainable);

    GIVEN("a mocked PWM")
    {
        PWM::Ptr pwm = std::make_unique<PWMMocked>(GPIOMocked::claim(12));

        WHEN("a Fan is constructed using this PWM and a max speed of 3000 RPM")
        {
            static constexpr double c_maxSpeedRpm = 3000.0;

            Fan::Ptr fan = std::make_unique<Fan>(std::move(pwm), c_maxSpeedRpm);

            THEN("we can access the max speed value")
            {
                REQUIRE(fan->getMaxSpeedRpm() == Approx(c_maxSpeedRpm));
            }

            THEN("we can write a new speed to the Fan")
            {
                static constexpr double c_speedRpm = 2000.0;
                REQUIRE_NOTHROW(fan->write(c_speedRpm));
            }

            THEN("a runtime_error exception is thrown if we try to read the Fan's speed")
            {
                REQUIRE_THROWS_AS(fan->read(), std::runtime_error);
            }
        }
    }
}

SCENARIO("A Fan object can be built for a 4-pin fan (PWM + tachometer)")
{
    // Disable logger
    g_logger.setVerbosity(Logger::Unattainable);

    GIVEN("a mocked PWM and a mocked Tachometer")
    {
        PWM::Ptr pwm = std::make_unique<PWMMocked>(GPIOMocked::claim(12));
        Tachometer::Ptr tachometer = std::make_unique<TachometerMocked>(GPIOMocked::claim(8));

        WHEN("a Fan is constructed using these and a max speed of 7200 RPM")
        {
            static constexpr double c_maxSpeedRpm = 7200.0;

            Fan::Ptr fan = std::make_unique<Fan>(std::move(pwm), std::move(tachometer), c_maxSpeedRpm);

            THEN("we can access the max speed value")
            {
                REQUIRE(fan->getMaxSpeedRpm() == Approx(c_maxSpeedRpm));
            }

            THEN("we can write a new speed to the Fan")
            {
                static constexpr double c_speedRpm = 5400.0;
                REQUIRE_NOTHROW(fan->write(c_speedRpm));
            }

            THEN("we can read the current speed from the Fan")
            {
                REQUIRE_NOTHROW(fan->read());
            }
        }
    }
}

SCENARIO("A Fan object allows reading a fan's current speed in RPM", "[fan][hw]")
{
    // Disable logger
    g_logger.setVerbosity(Logger::Unattainable);

    GIVEN("a Fan object with mocked PWM/Tachometer and a max speed of 4800 RPM")
    {
        static constexpr double c_maxSpeedRpm = 4800.0;

        PWM::Ptr pwm = std::make_unique<PWMMocked>(GPIOMocked::claim(12));

        Tachometer::Ptr tachometer = std::make_unique<TachometerMocked>(GPIOMocked::claim(8));
        TachometerMocked * rawTachometerPtr = dynamic_cast<TachometerMocked*>(tachometer.get());

        Fan::Ptr fan = std::make_unique<Fan>(std::move(pwm), std::move(tachometer), c_maxSpeedRpm);

        WHEN("the Tachometer is configured to return a value of 3200 RPM")
        {
            static constexpr double c_speedRpm = 3200.0;
            rawTachometerPtr->currentVal = c_speedRpm;

            THEN("reading the Fan's speed should also return 3200 RPM")
            {
                REQUIRE(fan->read() == Approx(c_speedRpm));
            }
        }
    }
}

SCENARIO("A Fan object allows controlling a fan's current speed in RPM", "[fan][hw]")
{
    // Disable logger
    g_logger.setVerbosity(Logger::Unattainable);

    GIVEN("a Fan object with mocked PWM/Tachometer and a max speed of 1600 RPM")
    {
        static constexpr double c_maxSpeedRpm = 4800.0;

        PWM::Ptr pwm = std::make_unique<PWMMocked>(GPIOMocked::claim(12));
        PWMMocked * rawPwmPtr = dynamic_cast<PWMMocked*>(pwm.get());

        Tachometer::Ptr tachometer = std::make_unique<TachometerMocked>(GPIOMocked::claim(8));

        Fan::Ptr fan = std::make_unique<Fan>(std::move(pwm), std::move(tachometer), c_maxSpeedRpm);

        WHEN("we write a negative value to the Fan")
        {
            static constexpr double c_speedRpm = -200.0;

            THEN("an invalid_argument exception is thrown")
            {
                REQUIRE_THROWS_AS(fan->write(c_speedRpm), std::invalid_argument);
            }
        }

        WHEN("we write 800 RPM to the Fan")
        {
            static constexpr double c_speedRpm = 1000.0;
            fan->write(c_speedRpm);

            THEN("the PWM's duty cycle should be set to 62.5% (= speedRpm / maxSpeedRpm)")
            {
                REQUIRE(rawPwmPtr->lastDutyCycle == Approx(c_speedRpm / c_maxSpeedRpm));
            }
        }

        WHEN("we write a value greater than the Fan's max speed")
        {
            static constexpr double c_speedRpm = 1.5 * c_maxSpeedRpm;

            THEN("an invalid_argument exception is thrown")
            {
                REQUIRE_THROWS_AS(fan->write(c_speedRpm), std::invalid_argument);
            }
        }
    }
}
