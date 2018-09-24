//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "external/wiringPi.h"

#include <chrono>
#include <thread>

//==============================================================================
class WiringPiMocked : public beewatch::external::IWiringPi
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

    //==============================================================================
    virtual void delay(int s) override { std::this_thread::sleep_for(std::chrono::seconds(s)); }
    virtual void delayMicroseconds(int us) override { std::this_thread::sleep_for(std::chrono::microseconds(us)); }

    virtual unsigned int micros(void) override { using namespace std::chrono; return (unsigned)duration_cast<microseconds>(steady_clock::now().time_since_epoch()).count(); }
};

static void mockCallback() { static int counter = 0; ++counter; }