//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "io/pwm.h"

//==============================================================================
class PWMMocked final : public beewatch::io::PWM
{
public:
    //==============================================================================
    using GPIO = beewatch::io::GPIO;

    //==============================================================================
    PWMMocked(GPIO::Ptr&& gpio) : PWM(std::move(gpio)) {};
    virtual ~PWMMocked() = default;

    //==============================================================================
    double lastDutyCycle = 0.0;

    virtual void write(double dutyCycle) override { lastDutyCycle = dutyCycle; };
};
