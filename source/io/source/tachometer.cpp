//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/tachometer.h"
#include "io/pigpio.h"

#include <chrono>

namespace beewatch
{
    namespace io
    {

        //================================================================
        Tachometer::Tachometer(GPIO::Ptr&& gpio, int pulsesPerRevolution)
            : _pulsesPerRevolution(pulsesPerRevolution)
        {
            if (!gpio)
            {
                throw std::invalid_argument("Received undefined GPIO");
            }

            // Take ownership of GPIO
            _gpio = std::move(gpio);

            // Configure GPIO
            PiGPIOLib::init();

            gpioSetMode(_gpio->getId(), PI_INPUT);
        }

        Tachometer::~Tachometer()
        {
        }


        //================================================================
        double Tachometer::read() const
        {
            // TODO
        }

    } // namespace io
} // namespace beewatch
