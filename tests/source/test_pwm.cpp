//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "io/pwm.h"
#include "algorithms.h"
#include "logging.h"

#include "catch.hpp"
#include "wiringPiMock.hpp"

#include <cstdlib>
#include <ctime>

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

using io::PWM;
using io::GPIO;

//==============================================================================
SCENARIO("PWMs can be used ", "[io]")
{
    static const int c_expectedPwmRange = 1024;

    static const int c_rpiBasePwmClock = 19'200'000;
    static const int c_expectedPwmClock = 20'000;
    static const int c_expectedPwmClockDiv = c_rpiBasePwmClock / c_expectedPwmClock;

    // Disable non-fatal logs
    logger.setVerbosity(Logger::Fatal);

    GIVEN("a GPIO incapable of hardware PWM (pin 4) with a mocked wiringPi implementation")
    {
        std::shared_ptr<WiringPiMocked> wiringPiMock = std::make_shared<WiringPiMocked>();
        GPIO::Ptr gpio = GPIO::claim(4, wiringPiMock);

        WHEN("a PWM object is constructed for that GPIO")
        {
            THEN("an invalid_argument exception is thrown")
            {
                REQUIRE_THROWS_AS(std::make_shared<PWM>(std::move(gpio)), std::invalid_argument);
            }
        }
    }

    GIVEN("a GPIO capable of hardware PWM (pin 12) with a mocked wiringPi implementation")
    {
        std::shared_ptr<WiringPiMocked> wiringPiMock = std::make_shared<WiringPiMocked>();
        GPIO::Ptr gpio = GPIO::claim(12, wiringPiMock);

        WHEN("a PWM object is constructed for that GPIO")
        {
            GPIO * rawGpioPtr = gpio.get();
            PWM::Ptr pwm = std::make_shared<PWM>(std::move(gpio));

            THEN("the GPIO should be consumed by the PWM")
            {
                REQUIRE(gpio == nullptr);
            }

            THEN("the GPIO should be set to PWM mode")
            {
                REQUIRE(rawGpioPtr->getMode() == GPIO::Mode::PWM);
            }

            THEN("the underlying library should be set to PWM mode")
            {
                REQUIRE(wiringPiMock->currentPinMode == wiringPiMock->c_pwmToneOutput);
            }

            THEN("the underlying library should be set to the standard PWM mode (mark:space)")
            {
                REQUIRE(wiringPiMock->currentPwmMode == wiringPiMock->c_pwmModeMs);
            }

            THEN("the underlying library should be set to a default range of 1024")
            {
                REQUIRE(wiringPiMock->currentPwmRange == c_expectedPwmRange);
            }

            THEN("the underlying library should be set to the clock divisor such that the clock frequency is 20 kHz")
            {
                REQUIRE(wiringPiMock->currentPwmClockDiv == c_expectedPwmClockDiv);
            }

            THEN("the underlying library should be set to use a duty cycle of 0% (no output)")
            {
                REQUIRE(wiringPiMock->currentValue == 0);
            }
        }

        AND_GIVEN("a PWM configured with that GPIO")
        {
            PWM::Ptr pwm = std::make_shared<PWM>(std::move(gpio));

            WHEN("a value (42%) is written to the PWM interface")
            {
                static constexpr double c_dutyCycle = 0.42;

                int prevPwmMode = wiringPiMock->currentPwmMode;
                int prevPwmRange = wiringPiMock->currentPwmRange;
                int prevPwmClockDiv = wiringPiMock->currentPwmClockDiv;

                pwm->write(c_dutyCycle);

                THEN("the PWM should store the requested duty cycle")
                {
                    REQUIRE(pwm->getDutyCycle() == Approx(c_dutyCycle));
                }

                THEN("the underlying library should receive 42% of the PWM range (1024)")
                {
                    REQUIRE(wiringPiMock->currentValue == int(c_dutyCycle * c_expectedPwmRange));
                }

                THEN("no parameter other than the current value should be changed")
                {
                    REQUIRE(wiringPiMock->currentPwmMode == prevPwmMode);
                    REQUIRE(wiringPiMock->currentPwmRange == prevPwmRange);
                    REQUIRE(wiringPiMock->currentPwmClockDiv == prevPwmClockDiv);
                }
            }
        }
    }
}