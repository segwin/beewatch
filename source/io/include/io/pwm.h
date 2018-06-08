//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <io/io_api.h>

#include "io.h"
#include "gpio.h"

#include <map>

namespace beewatch
{
    namespace io
    {

        //================================================================
        /**
         * @class PWMOut
         *
         * Models a PWM output interface
         */
        class IO_API PWM : public InputOutput<double>
        {
        public:
            //================================================================
            /**
             * @enum PWM channel ID
             */
            enum class ID
            {
                /*PWM0 = GPIO::Fn::PWM0,
                PWM1 = GPIO::Fn::PWM1*/
            };

            //================================================================
            /**
             * @brief Construct a PWM object on a given GPIO
             *
             * NB: Consumes the given GPIO. It will be released on object destruction.
             *
             * @param [in] gpio         GPIO to use for PWM
             * @param [in] pwmId        PWM channel ID
             * @param [in] pwmFreqHz    PWM target frequency (Hz)
             */
            PWM(GPIO::Ptr&& gpio, ID pwmId, unsigned pwmFreqHz);

            /**
             * @brief Destroy PWM object, releasing owned GPIO
             */
            virtual ~PWM();

            //================================================================
            /**
             * Shared pointer to a PWM object
             */
            using Ptr = std::shared_ptr<PWM>;

            //================================================================
            /**
             * @brief Get the next available GPIO with the requested PWM channel
             *
             * Searches all GPIOs for the first available match with the given
             * PWM channel. If no such GPIO is available, returns nullptr.
             * 
             * @param [in] pwmId    PWM channel to search for
             *
             * @returns GPIO object if successful, nullptr otherwise
             */
            static GPIO::Ptr getGPIO(ID pwmId);

            //================================================================
            /**
             * @brief Get the current PWM duty cycle
             *
             * @returns Current duty cycle expressed as a ratio between 0 and 1
             */
            virtual double read() override;

            /**
             * @brief Set the PWM duty cycle to the given ratio
             *
             * @param [in] dutyCycle    Duty cycle expressed as a ratio between 0 and 1
             */
            virtual void write(double dutyCycle) override;

        private:
            //================================================================
            GPIO::Ptr _gpio;

            //================================================================
            const unsigned _pwmFreqHz;

            double _dutyCycle;
        };

    } // namespace io
} // namespace beewatch
