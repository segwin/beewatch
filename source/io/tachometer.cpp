//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/tachometer.h"

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
            _gpio->setMode(GPIO::Mode::Input);
        }

        Tachometer::~Tachometer()
        {
        }


        //================================================================
        double Tachometer::read()
        {
            double val = -1.0;

            // TODO

            return val;
        }

    } // namespace io
} // namespace beewatch
