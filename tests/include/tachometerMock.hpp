//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "io/tachometer.h"

//==============================================================================
class TachometerMocked final : public beewatch::io::Tachometer
{
public:
    //==============================================================================
    using GPIO = beewatch::io::GPIO;

    //==============================================================================
    TachometerMocked(GPIO::Ptr&& gpio) : Tachometer(std::move(gpio)) {};
    virtual ~TachometerMocked() = default;

    //==============================================================================
    double currentVal;

    virtual double read() override { return currentVal; }
};
