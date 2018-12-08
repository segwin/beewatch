//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "external/wiringPi.h"

#include "util/logging.h"

#ifdef HAS_WIRINGPI
#include <wiringPi.h>
#else
#include <thread>

// Mock wiringPi definitions: not intended to accurately reflect actual library values
#define INPUT               0
#define OUTPUT              1
#define GPIO_CLOCK          2
#define PWM_TONE_OUTPUT     3

#define LOW                 0
#define HIGH                1

#define PUD_OFF             0
#define PUD_DOWN            1
#define PUD_UP              2

#define PWM_MODE_MS         0
#define PWM_MODE_BAL        1

#define INT_EDGE_SETUP      0
#define INT_EDGE_FALLING    1
#define INT_EDGE_RISING     2
#define INT_EDGE_BOTH       3
#endif

#ifndef HAS_WIRINGPI
#ifdef __linux__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#else
#pragma warning(push)
#pragma warning(disable: 4100)
#endif
#endif

namespace beewatch::external
{

    //==============================================================================
    IWiringPi::IWiringPi(int low, int high,
                         int input, int output, int pwmToneOutput, int gpioClock,
                         int pwmModeMs, int pwmModeBal,
                         int pudOff, int pudUp, int pudDown,
                         int intEdgeSetup, int intEdgeRising, int intEdgeFalling, int intEdgeBoth)
        : c_low(low), c_high(high),
          c_input(input), c_output(output), c_pwmToneOutput(pwmToneOutput), c_gpioClock(gpioClock),
          c_pwmModeMs(pwmModeMs), c_pwmModeBal(pwmModeBal),
          c_pudOff(pudOff), c_pudUp(pudUp), c_pudDown(pudDown),
          c_intEdgeSetup(intEdgeSetup), c_intEdgeRising(intEdgeRising), c_intEdgeFalling(intEdgeFalling), c_intEdgeBoth(intEdgeBoth)
    {
    }

    //==============================================================================
    WiringPi::WiringPi()
        : IWiringPi(LOW, HIGH,
                    INPUT, OUTPUT, PWM_TONE_OUTPUT, GPIO_CLOCK,
                    PWM_MODE_MS, PWM_MODE_BAL,
                    PUD_OFF, PUD_UP, PUD_DOWN,
                    INT_EDGE_SETUP, INT_EDGE_RISING, INT_EDGE_FALLING, INT_EDGE_BOTH)
    {
#ifdef HAS_WIRINGPI
        if (::wiringPiSetupGpio() < 0)
        {
            g_logger.print(Logger::Fatal, "An error occurred while initialising wiringPi library, do we have root privileges?");
            exit(-1);
        }
#else
        _startTime = std::chrono::steady_clock::now();
#endif
    }

    //==============================================================================
    std::shared_ptr<IWiringPi> WiringPi::getInstance()
    {
        // Expose private default constructor to std::make_shared through a
        // derived class with public default constructor
        struct make_shared_enabler : public WiringPi {};
        static std::shared_ptr<IWiringPi> instance = std::make_shared<make_shared_enabler>();

        return instance;
    }

    //==============================================================================
    int WiringPi::digitalRead(int pin)
    {
#ifdef HAS_WIRINGPI
        return ::digitalRead(pin);
#else
        return 0;
#endif
    }

    void WiringPi::digitalWrite(int pin, int value)
    {
#ifdef HAS_WIRINGPI
        ::digitalWrite(pin, value);
#endif
    }

    void WiringPi::pwmWrite(int pin, int value)
    {
#ifdef HAS_WIRINGPI
        ::pwmWrite(pin, value);
#endif
    }

    //==============================================================================
    void WiringPi::pinMode(int pin, int mode)
    {
#ifdef HAS_WIRINGPI
        ::pinMode(pin, mode);
#endif
    }

    void WiringPi::pullUpDnControl(int pin, int pud)
    {
#ifdef HAS_WIRINGPI
        ::pullUpDnControl(pin, pud);
#endif
    }

    void WiringPi::pwmSetMode(int mode)
    {
#ifdef HAS_WIRINGPI
        ::pwmSetMode(mode);
#endif
    }

    void WiringPi::pwmSetRange(unsigned int range)
    {
#ifdef HAS_WIRINGPI
        ::pwmSetRange(range);
#endif
    }

    void WiringPi::pwmSetClock(int divisor)
    {
#ifdef HAS_WIRINGPI
        ::pwmSetClock(divisor);
#endif
    }

    //==============================================================================
    int WiringPi::setISR(int pin, int edgeType, void(*function)(void))
    {
#ifdef HAS_WIRINGPI
        return ::wiringPiISR(pin, edgeType, function);
#else
        return 0;
#endif
    }

} // namespace beewatch::external

#ifndef HAS_WIRINGPI
#ifdef __linux__
#pragma GCC diagnostic pop
#else
#pragma warning(pop)
#endif
#endif
