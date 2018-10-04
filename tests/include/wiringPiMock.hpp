//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "external/wiringPi.h"

#include <chrono>
#include <thread>

//==============================================================================
class WiringPiMocked final : public beewatch::external::IWiringPi
{
public:
    //==============================================================================
    WiringPiMocked() = default;
    virtual ~WiringPiMocked() = default;

    //==============================================================================
    int lastPin = rand();
    int currentValue = rand();

    virtual int digitalRead(int pin) override { lastPin = pin; return currentValue; }
    virtual void digitalWrite(int pin, int value) override { lastPin = pin; currentValue = value; }

    virtual void pwmWrite(int pin, int value) override { lastPin = pin; currentValue = value; };

    //==============================================================================
    int currentPinMode = rand();
    virtual void pinMode(int pin, int mode) override { lastPin = pin; currentPinMode = mode; }

    int currentPudMode = rand();
    virtual void pullUpDnControl(int pin, int pud) override { lastPin = pin; currentPudMode = pud; }

    int currentPwmMode = rand();
    virtual void pwmSetMode(int mode) override { currentPwmMode = mode; }

    int currentPwmRange = rand();
    virtual void pwmSetRange(unsigned int range) override { currentPwmRange = range; }

    int currentPwmClockDiv = rand();
    virtual void pwmSetClock(int divisor) override { currentPwmClockDiv = divisor; }

    //==============================================================================
    int currentEdgeType = rand();
    void(*currentISR)(void);

    virtual int setISR(int pin, int edgeType, void(*function)(void)) override { lastPin = pin; currentEdgeType = edgeType; currentISR = function; return 0; }
};
