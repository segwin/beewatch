//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "io/gpio.h"
#include "algorithms.h"
#include "logging.h"

#include "catch.hpp"

#include <cstdlib>
#include <ctime>

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

using io::LogicalState;
using io::GPIO;

//==============================================================================
SCENARIO("GPIOs can be claimed and released", "[gpio]")
{
    // Disable non-fatal logs
    logger.setVerbosity(Logger::Fatal);

    GIVEN("all GPIOs are initially free")
    {
        REQUIRE(GPIO::getAvailableIDs().size() == GPIO::NUM_GPIO);

        WHEN("we claim a valid GPIO")
        {
            GPIO::Ptr gpio = GPIO::claim(4);

            THEN("we receive a unique pointer to that GPIO with the default config")
            {
                REQUIRE(gpio != nullptr);
                REQUIRE(gpio->getID() == 4);
                REQUIRE(gpio->getMode() == GPIO::Mode::Input);
            }

            THEN("that GPIO can no longer be claimed")
            {
                GPIO::Ptr sameGPIO = GPIO::claim(4);
                REQUIRE(sameGPIO == nullptr);
            }

            AND_WHEN("we set the GPIO's mode, then release it")
            {
                gpio->setMode(GPIO::Mode::Output);

                REQUIRE(gpio->getMode() == GPIO::Mode::Output);

                gpio.reset();

                THEN("all GPIOs are free again, including the released GPIO")
                {
                    std::vector<int> availableIDs = GPIO::getAvailableIDs();

                    REQUIRE(availableIDs.size() == GPIO::NUM_GPIO);
                    REQUIRE(contains(availableIDs.begin(), availableIDs.end(), 4));
                }
                
                THEN("it can be claimed again and has the default config")
                {
                    GPIO::Ptr sameGPIO = GPIO::claim(4);

                    REQUIRE(sameGPIO != nullptr);
                    REQUIRE(sameGPIO->getID() == 4);
                    REQUIRE(sameGPIO->getMode() == GPIO::Mode::Input);
                }
            }
        }

        WHEN("we try to claim a GPIO with a negative ID")
        {
            bool caughtRangeErr = false;

            try
            {
                GPIO::Ptr gpio = GPIO::claim(-1);
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
                GPIO::Ptr gpio = GPIO::claim(GPIO::NUM_GPIO);
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
        std::vector<GPIO::Ptr> claimedGPIOs;

        for (int i = 0; i < GPIO::NUM_GPIO; ++i)
        {
            claimedGPIOs.push_back(GPIO::claim(i));
        }

        REQUIRE(GPIO::getAvailableIDs().empty());

        WHEN("we check their IDs")
        {
            THEN("all GPIOs have the expected ID values")
            {
                REQUIRE(claimedGPIOs.size() == GPIO::NUM_GPIO);

                for (int i = 0; i < GPIO::NUM_GPIO; ++i)
                {
                    REQUIRE(claimedGPIOs[i]->getID() == i);
                }
            }
        }

        WHEN("we try to claim any GPIO")
        {
            GPIO::Ptr gpio = GPIO::claim(12);

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
                GPIO::Ptr gpio = GPIO::claim(12);

                REQUIRE(gpio != nullptr);
                REQUIRE(gpio->getID() == 12);
            }
        }
    }
}

//==============================================================================
class WiringPiMocked : public external::IWiringPi
{
public:
    //================================================================
    WiringPiMocked() = default;
    virtual ~WiringPiMocked() = default;

    //================================================================
    int lastPin = rand();
    int currentValue = rand();

    virtual int digitalRead(int pin) override { lastPin = pin; return currentValue; }
    virtual void digitalWrite(int pin, int value) override { lastPin = pin; currentValue = value; }

    virtual void pwmWrite(int pin, int value) override { lastPin = pin; currentValue = value; };

    //================================================================
    int currentPinMode = rand();
    virtual void pinMode(int pin, int mode) override { lastPin = pin; currentPinMode = mode; }

    int currentPudMode = rand();
    virtual void pullUpDnControl(int pin, int pud) override { lastPin = pin; currentPudMode = pud; }

    int currentPwmMode = rand();
    virtual void pwmSetMode(int mode) override { currentPwmMode = mode; }

    int currentPwmRange = rand();
    virtual void pwmSetRange(unsigned int range) override { currentPwmRange = range; }

    int currentPwmClock = rand();
    virtual void pwmSetClock(int divisor) override { currentPwmClock = divisor; }

    //================================================================
    int currentEdgeType = rand();
    void(*currentISR)(void);

    virtual int setISR(int pin, int edgeType, void(*function)(void)) override { lastPin = pin; currentEdgeType = edgeType; currentISR = function; return 0; }

    //================================================================
    virtual void delay(int s) override { std::this_thread::sleep_for(std::chrono::seconds(s)); }
    virtual void delayMicroseconds(int us) override { std::this_thread::sleep_for(std::chrono::microseconds(us)); }

    virtual unsigned int micros(void) override { using namespace std::chrono; return (unsigned)duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count(); }
};

void mockCallback() { static int counter = 0; ++counter; }

SCENARIO("GPIOs can configure, read & write pins", "[gpio]")
{

    GIVEN("a GPIO with a mocked wiringPi implementation")
    {
        srand(static_cast<unsigned>(time(0)));

        std::shared_ptr<WiringPiMocked> wiringPiMock = std::make_shared<WiringPiMocked>();
        GPIO::Ptr gpio = GPIO::claim(4, wiringPiMock);

        REQUIRE(gpio != nullptr);
        REQUIRE(gpio->getID() == 4); 

        // Set mode
        WHEN("we set the GPIO to Input mode")
        {
            gpio->setMode(GPIO::Mode::Input);

            THEN("the mode should be set to Input on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getMode() == GPIO::Mode::Input);
                REQUIRE(wiringPiMock->currentPinMode == wiringPiMock->c_input);
            }
        }

        WHEN("we set the GPIO to Output mode")
        {
            gpio->setMode(GPIO::Mode::Output);

            THEN("the mode should be set to Output on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getMode() == GPIO::Mode::Output);
                REQUIRE(wiringPiMock->currentPinMode == wiringPiMock->c_output);
            }
        }

        WHEN("we set the GPIO to PWM mode")
        {
            gpio->setMode(GPIO::Mode::PWM);

            THEN("the mode should be set to PWM on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getMode() == GPIO::Mode::PWM);
                REQUIRE(wiringPiMock->currentPinMode == wiringPiMock->c_pwmToneOutput);
            }
        }

        WHEN("we set the GPIO to Clock mode")
        {
            gpio->setMode(GPIO::Mode::CLK);

            THEN("the mode should be set to Clock on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getMode() == GPIO::Mode::CLK);
                REQUIRE(wiringPiMock->currentPinMode == wiringPiMock->c_gpioClock);
            }
        }

        // Resistor config
        WHEN("we set the resistor to off")
        {
            gpio->setResistorMode(GPIO::Resistor::Off);

            THEN("the resistor should be set to Off on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getResistorMode() == GPIO::Resistor::Off);
                REQUIRE(wiringPiMock->currentPudMode == wiringPiMock->c_pudOff);
            }
        }

        WHEN("we set the resistor to PullUp")
        {
            gpio->setResistorMode(GPIO::Resistor::PullUp);

            THEN("the resistor should be set to PullUp on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getResistorMode() == GPIO::Resistor::PullUp);
                REQUIRE(wiringPiMock->currentPudMode == wiringPiMock->c_pudUp);
            }
        }

        WHEN("we set the resistor to PullDown")
        {
            gpio->setResistorMode(GPIO::Resistor::PullDown);

            THEN("the resistor should be set to PullDown on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getResistorMode() == GPIO::Resistor::PullDown);
                REQUIRE(wiringPiMock->currentPudMode == wiringPiMock->c_pudDown);
            }
        }

        // Edge detection
        WHEN("we set the edge detection to Rising with some ISR callback")
        {
            gpio->setEdgeDetection(GPIO::EdgeType::Rising, &mockCallback);

            THEN("the ISR callback should match the given method")
            {
                REQUIRE(wiringPiMock->currentISR == &mockCallback);
            }

            THEN("the edge detection should be set to Rising on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getEdgeDetection() == GPIO::EdgeType::Rising);
                REQUIRE(wiringPiMock->currentEdgeType == wiringPiMock->c_intEdgeRising);
            }
        }

        WHEN("we set the edge detection to Falling with some ISR callback")
        {
            gpio->setEdgeDetection(GPIO::EdgeType::Falling, &mockCallback);

            THEN("the ISR callback should match the given method")
            {
                REQUIRE(wiringPiMock->currentISR == &mockCallback);
            }

            THEN("the edge detection should be set to Falling on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getEdgeDetection() == GPIO::EdgeType::Falling);
                REQUIRE(wiringPiMock->currentEdgeType == wiringPiMock->c_intEdgeFalling);
            }
        }

        WHEN("we set the edge detection to Both with some ISR callback")
        {
            gpio->setEdgeDetection(GPIO::EdgeType::Both, &mockCallback);

            THEN("the ISR callback should match the given method")
            {
                REQUIRE(wiringPiMock->currentISR == &mockCallback);
            }

            THEN("the edge detection should be set to Both on the GPIO and in the underlying library")
            {
                REQUIRE(gpio->getEdgeDetection() == GPIO::EdgeType::Both);
                REQUIRE(wiringPiMock->currentEdgeType == wiringPiMock->c_intEdgeBoth);
            }
        }

        WHEN("we set the edge detection to None with some ISR callback")
        {
            gpio->setEdgeDetection(GPIO::EdgeType::None, &mockCallback);

            THEN("the ISR callback should be set to GPIO::nop()")
            {
                REQUIRE(wiringPiMock->currentISR == &GPIO::nop);
            }

            THEN("the edge detection should be set to None on the GPIO and Rising (default) in the underlying library")
            {
                REQUIRE(gpio->getEdgeDetection() == GPIO::EdgeType::None);
                REQUIRE(wiringPiMock->currentEdgeType == wiringPiMock->c_intEdgeRising);
            }
        }

        // Read
        AND_GIVEN("the GPIO is configured as input")
        {
            gpio->setMode(GPIO::Mode::Input);

            REQUIRE(gpio->getMode() == GPIO::Mode::Input);
            REQUIRE(wiringPiMock->currentPinMode == wiringPiMock->c_input);

            WHEN("we set the simulated pin to HIGH and read the GPIO")
            {
                wiringPiMock->currentValue = wiringPiMock->c_high;
                LogicalState state = gpio->read();

                THEN("we should receive LogicalState::HI as output")
                {
                    REQUIRE(state == LogicalState::HI);
                }
            }

            WHEN("we set the simulated pin to LOW and read the GPIO")
            {
                wiringPiMock->currentValue = wiringPiMock->c_low;
                LogicalState state = gpio->read();

                THEN("we should receive LogicalState::LO as output")
                {
                    REQUIRE(state == LogicalState::LO);
                }
            }

            WHEN("we set the simulated pin to an invalid state and read the GPIO")
            {
                wiringPiMock->currentValue = -999;
                LogicalState state = gpio->read();

                THEN("we should receive LogicalState::Invalid as output")
                {
                    REQUIRE(state == LogicalState::Invalid);
                }
            }

            WHEN("we attempt to write to the input GPIO")
            {
                bool caughtInvalidArgument = false;

                try
                {
                    gpio->write(LogicalState::HI);
                }
                catch (std::invalid_argument)
                {
                    caughtInvalidArgument = true;
                }

                THEN("an invalid_argument exception should be thrown")
                {
                    REQUIRE(caughtInvalidArgument);
                }
            }
        }

        // Write
        AND_GIVEN("the GPIO is configured as input")
        {
            gpio->setMode(GPIO::Mode::Output);

            REQUIRE(gpio->getMode() == GPIO::Mode::Output);
            REQUIRE(wiringPiMock->currentPinMode == wiringPiMock->c_output);

            WHEN("we write LogicalState::HI on the GPIO")
            {
                gpio->write(LogicalState::HI);

                THEN("the simulated pin should be set to HIGH")
                {
                    REQUIRE(wiringPiMock->currentValue == wiringPiMock->c_high);
                }
            }

            WHEN("we write LogicalState::LO on the GPIO")
            {
                gpio->write(LogicalState::LO);

                THEN("the simulated pin should be set to LOW")
                {
                    REQUIRE(wiringPiMock->currentValue == wiringPiMock->c_low);
                }
            }

            WHEN("we write LogicalState::Invalid on the GPIO")
            {
                bool caughtInvalidArgument = false;

                try
                {
                    gpio->write(LogicalState::Invalid);
                }
                catch (std::invalid_argument)
                {
                    caughtInvalidArgument = true;
                }

                THEN("an invalid_argument exception should be thrown")
                {
                    REQUIRE(caughtInvalidArgument);
                }
            }

            WHEN("we attempt to read from the output GPIO")
            {
                bool caughtInvalidArgument = false;

                try
                {
                    gpio->read();
                }
                catch (std::invalid_argument)
                {
                    caughtInvalidArgument = true;
                }

                THEN("an invalid_argument exception should be thrown")
                {
                    REQUIRE(caughtInvalidArgument);
                }
            }
        }
    }
}