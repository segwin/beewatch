//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "patterns.h"

#ifndef HAS_WIRINGPI
#include <chrono>
#endif

#include <memory>

namespace beewatch::external
{

    //==============================================================================
    /**
     * @interface IWiringPi
     */
    class IWiringPi : public unique_ownership_t<IWiringPi>
    {
    protected:
        /**
         * @brief Construct interface and set constant values
         */
        IWiringPi(int low, int high,
                  int input, int output, int pwmToneOutput, int gpioClock,
                  int pwmModeMs, int pwmModeBal,
                  int pudOff, int pudUp, int pudDown,
                  int intEdgeSetup, int intEdgeRising, int intEdgeFalling, int intEdgeBoth);

    public:
        //==============================================================================
        // Logical states
        int c_low = 0;
        int c_high = 1;

        // Pin modes
        int c_input = 0;
        int c_output = 1;
        int c_pwmToneOutput = 2;
        int c_gpioClock = 3;

        // PWM modes
        int c_pwmModeMs = 0;
        int c_pwmModeBal = 1;

        // Resistor modes
        int c_pudOff = 0;
        int c_pudUp = 1;
        int c_pudDown = 2;

        // Edge detection modes
        int c_intEdgeSetup = 0;
        int c_intEdgeRising = 1;
        int c_intEdgeFalling = 2;
        int c_intEdgeBoth = 3;

        //==============================================================================
        IWiringPi() = default;
        virtual ~IWiringPi() = default;

        //==============================================================================
        virtual int digitalRead(int pin) = 0;
        virtual void digitalWrite(int pin, int value) = 0;

        virtual void pwmWrite(int pin, int value) = 0;

        //==============================================================================
        virtual void pinMode(int pin, int mode) = 0;
        virtual void pullUpDnControl(int pin, int pud) = 0;

        virtual void pwmSetMode(int mode) = 0;
        virtual void pwmSetRange(unsigned int range) = 0;
        virtual void pwmSetClock(int divisor) = 0;

        //==============================================================================
        virtual int setISR(int pin, int edgeType, void(*function)(void)) = 0;
    };

    //==============================================================================
    /**
     * @class WiringPi
     *
     * Implements the wiringPi library interface
     */
    class WiringPi : public IWiringPi
    {
        //==============================================================================
        WiringPi();

#ifndef HAS_WIRINGPI
        //==============================================================================
        std::chrono::steady_clock::time_point _startTime;
#endif

    public:
        //==============================================================================
        static std::shared_ptr<IWiringPi> getInstance();

        virtual ~WiringPi() = default;

        //==============================================================================
        virtual int digitalRead(int pin) override;
        virtual void digitalWrite(int pin, int value) override;

        virtual void pwmWrite(int pin, int value) override;

        //==============================================================================
        virtual void pinMode(int pin, int mode) override;
        virtual void pullUpDnControl(int pin, int pud) override;

        virtual void pwmSetMode(int mode) override;
        virtual void pwmSetRange(unsigned int range) override;
        virtual void pwmSetClock(int divisor) override;

        //==============================================================================
        virtual int setISR(int pin, int edgeType, void(*function)(void)) override;
    };

}

