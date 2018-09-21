//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "hw/fan.h"

namespace beewatch
{
    namespace hw
    {

        //================================================================
        Fan::Fan(io::GPIO::Ptr&& fanGpio, io::GPIO::Ptr&& tachometerGpio, double maxSpeedRpm)
            : _pwm(std::move(fanGpio)),
              _tachometer(std::move(tachometerGpio), 2),
              _maxSpeedRpm(maxSpeedRpm)
        {
        }

        Fan::~Fan() = default;

        //================================================================
        double Fan::read()
        {
            return _tachometer.read();
        }

        void Fan::write(double speedRpm)
        {
            return _pwm.write(_maxSpeedRpm / speedRpm);
        }


    } // namespace hw
} // namespace beewatch
