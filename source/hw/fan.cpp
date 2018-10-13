//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "hw/fan.h"

namespace beewatch::hw
{

    //==============================================================================
    Fan::Fan(io::PWM::Ptr&& pwmOut, double maxSpeedRpm)
        : _maxSpeedRpm(maxSpeedRpm)
    {
        _pwm = std::move(pwmOut);
    }

    Fan::Fan(io::PWM::Ptr&& pwmOut, io::Tachometer::Ptr&& tachometerIn, double maxSpeedRpm)
        : Fan(std::move(pwmOut), maxSpeedRpm)
    {
        _tachometer = std::move(tachometerIn);
    }

    Fan::~Fan() = default;

    //==============================================================================
    double Fan::read()
    {
        if (!_tachometer)
        {
            throw std::runtime_error("Called read() on a Fan without a Tachometer!");
        }

        return _tachometer->read();
    }

    void Fan::write(double speedRpm)
    {
        return _pwm->write(_maxSpeedRpm / speedRpm);
    }

} // namespace beewatch::hw
