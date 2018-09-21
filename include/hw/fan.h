//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include "io/gpio.h"
#include "io/io.h"
#include "io/pwm.h"
#include "io/tachometer.h"

#include <memory>

namespace beewatch
{
    namespace hw
    {

        //================================================================
        /**
         * @class Fan
         *
         * Models a 4-pin PWM fan
         */
        class Fan : public io::InputOutput<double>
        {
        public:
            //================================================================
            /**
             * Unique pointer to a Fan object
             */
            using Ptr = std::unique_ptr<Fan>;


            //================================================================
            /**
             * @brief Construct Fan object
             *
             * @param [in] fanGpio          GPIO connected to fan's PWM input
             * @param [in] tachometerGpio   GPIO connected to fan's tachometer output
             * @param [in] maxSpeedRpm      Fan's max speed in RPM
             */
            Fan(io::GPIO::Ptr&& fanGpio, io::GPIO::Ptr&& tachometerGpio, double maxSpeedRpm);

            /**
             * @brief Destroy Fan object
             */
            virtual ~Fan();


            //================================================================
            /**
             * @brief Read current fan speed
             *
             * @returns Current fan speed in RPM
             */
            double read() override;

            /**
             * @brief Write new fan speed
             *
             * @param [in] speedRpm New fan speed in RPM
             */
            void write(double speedRpm) override;


            //================================================================
            double getMaxSpeedRpm() { return _maxSpeedRpm; }


        protected:
            //================================================================
            io::PWM _pwm;
            io::Tachometer _tachometer;

            double _maxSpeedRpm;
        };

    } // namespace hw
} // namespace beewatch
