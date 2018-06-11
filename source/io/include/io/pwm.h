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
         * @class PWM
         *
         * Models a PWM output interface
         */
        class IO_API PWM : public Output<double>
        {
        public:
            //================================================================
            /**
             * @brief Construct a PWM object on a given GPIO
             *
             * NB: Consumes the given GPIO. It will be released on object destruction.
             *
             * @param [in] gpio         GPIO to use for PWM
             * @param [in] pwmFreqHz    PWM target frequency (Hz)
             */
            PWM(GPIO::Ptr&& gpio);

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
             * @brief Set the PWM duty cycle to the given ratio
             *
             * @param [in] dutyCycle    Duty cycle expressed as a ratio between 0 and 1
             */
            virtual void write(double dutyCycle) override;

            /**
             * @brief Get duty cycle from last write()
             *
             * @returns Current duty cycle
             */
            double getDutyCycle() const { return _dutyCycle; }


        private:
            //================================================================
            GPIO::Ptr _gpio;

            //================================================================
            static constexpr int PWM_RANGE = 1024;
            
            static constexpr int PWM_CLOCK_MAX_HZ = 19'200'000;
            static constexpr int PWM_CLOCK_HZ= 20'000;

            double _dutyCycle;
        };

    } // namespace io
} // namespace beewatch
