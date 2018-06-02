//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/pwm.h"
#include "io/pigpio.h"

#include "pigpio/pigpio.h"

#include <string>

namespace beewatch
{
    namespace io
    {

        //================================================================
        PWM::PWM(GPIO::Ptr&& gpio, ID pwmId, unsigned pwmFreqHz)
            : _pwmFreqHz(pwmFreqHz), _dutyCycle(0.0)
        {
            if (!gpio)
            {
                throw std::invalid_argument("Received undefined GPIO");
            }

            // Take ownership of GPIO
            _gpio = std::move(gpio);

            // Given GPIO must be capable of hardware PWM
            // NB: We can cast PWM::ID to GPIO::Fn since we've defined the
            //     former to be equal in value to the latter
            int gpioPwmMode = _gpio->findFunction((GPIO::Fn)pwmId);
            
            if (gpioPwmMode < 0)
            {
                throw std::invalid_argument("Requested GPIO is not capable of hardware PWM");
            }

            // Configure PWM output
            PiGPIOLib::init();

            _gpio->setMode(gpioPwmMode);
            write(0.5);
        }

        PWM::~PWM()
        {
        }


        //================================================================
        GPIO::Ptr PWM::getGPIO(ID pwmId)
        {
            for (int gpioId = 0; gpioId < GPIO::NUM_GPIO; ++gpioId)
            {
                int gpioPwmMode = GPIO::findFunction(gpioId, (GPIO::Fn)pwmId);

                if (gpioPwmMode >= 0)
                {
                    auto gpio = GPIO::claim(gpioId);

                    // Continue loop if this GPIO is already taken (the same
                    // function is available across many GPIOs)
                    if (gpio != nullptr)
                    {
                        return gpio;
                    }
                }
            }

            // If we reach this point, no GPIO with the requested PWM function
            // was found or was available
            return nullptr;
        }


        //================================================================
        double PWM::read()
        {
            unsigned gpioId = _gpio->getId();

            int pwmValue = gpioGetPWMdutycycle(gpioId);
            int range = gpioGetPWMrange(gpioId);

            if (pwmValue < 0 || range < 0)
            {
                throw std::invalid_argument("Invalid GPIO (" +
                                            std::to_string(gpioId) +
                                            ")");
            }

            return pwmValue / range;
        }

        void PWM::write(double dutyCycle)
        {
            unsigned gpioId = _gpio->getId();

            double range = gpioGetPWMrange(gpioId);
            unsigned pwmValue = dutyCycle * range;

            int rc = gpioHardwarePWM(gpioId, _pwmFreqHz, pwmValue);

            if (rc == PI_BAD_USER_GPIO)
            {
                throw std::invalid_argument("Invalid GPIO (" +
                                            std::to_string(gpioId) +
                                            ")");
            }
            else if (rc == PI_BAD_DUTYCYCLE)
            {
                throw std::invalid_argument("Invalid duty cycle (" +
                                            std::to_string(dutyCycle) +
                                            ")");
            }
        }
        
    } // namespace io
} // namespace beewatch
