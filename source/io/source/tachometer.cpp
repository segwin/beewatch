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

            _gpio->setDirection(GPIO::In);
        }

        Tachometer::~Tachometer()
        {
        }


        //================================================================
        double Tachometer::read()
        {
            // TODO
        }

    } // namespace io
} // namespace beewatch
