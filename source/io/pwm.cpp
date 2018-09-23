//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/pwm.h"

#include <external/wiringPi.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <string>

namespace beewatch
{
    namespace io
    {

        //================================================================
        static constexpr std::array<int, 7> pwmCapablePins{ { 12, 13, 18, 19, 40, 41, 45 } };


        //================================================================
        PWM::PWM(GPIO::Ptr&& gpio)
            : _dutyCycle(0.0)
        {
            if (!gpio)
            {
                throw std::invalid_argument("Received undefined GPIO");
            }
            
            // Given GPIO must be capable of hardware PWM
            bool isPwmCapable = std::any_of( pwmCapablePins.begin(), pwmCapablePins.end(),
                                             [&](int i) { return i == gpio->getID(); } );

            if (!isPwmCapable)
            {
                throw std::invalid_argument("Given GPIO is not capable of hardware PWM, expected pin 12");
            }

            // Take ownership of GPIO
            _gpio = std::move(gpio);
            
            // Configure PWM output
            _gpio->setMode(GPIO::Mode::PWM);
            
            _gpio->getWiringPi()->pwmSetMode(_gpio->getWiringPi()->c_pwmModeMs);
            _gpio->getWiringPi()->pwmSetRange(PWM_RANGE);
            _gpio->getWiringPi()->pwmSetClock(PWM_CLOCK_MAX_HZ / PWM_CLOCK_HZ);

            write(_dutyCycle);
        }

        PWM::~PWM() = default;


        //================================================================
        void PWM::write(double dutyCycle)
        {
            assert(dutyCycle >= 0.0);
            assert(dutyCycle <= 1.0);

            _gpio->getWiringPi()->pwmWrite(_gpio->getID(), static_cast<int>(dutyCycle * PWM_RANGE));
        }
        
    } // namespace io
} // namespace beewatch
