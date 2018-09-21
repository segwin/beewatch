//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include "io.h"
#include "gpio.h"

#include <atomic>
#include <thread>

namespace beewatch
{
    namespace io
    {

        //================================================================
        /**
         * @class Tachometer
         *
         * Models a tachometer interface
         */
        class Tachometer : public Input<double>
        {
        public:
            //================================================================
            /**
             * @brief Construct a Tachometer object on a given GPIO
             *
             * NB: Consumes the given GPIO. It will be released on object destruction.
             *
             * @param [in] gpio                 GPIO to use for PWM
             * @param [in] pulsesPerRevolution  Number of pulses counted per revolution
             */
            Tachometer(GPIO::Ptr&& gpio, int pulsesPerRevolution);
            
            /**
             * @brief Destroy Tachometer object, releasing owned GPIO
             */
            virtual ~Tachometer();

            //================================================================
            /**
             * Shared pointer to a Tachometer object
             */
            using Ptr = std::shared_ptr<Tachometer>;

            //================================================================
            virtual double read() override;

        protected:
            //================================================================
            GPIO::Ptr _gpio;

            //================================================================
            const int _pulsesPerRevolution;

            double _speedRpm;
        };

    } // namespace io
} // namespace beewatch
