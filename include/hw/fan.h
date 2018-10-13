//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "io/io.h"
#include "io/pwm.h"
#include "io/tachometer.h"

#include <memory>

namespace beewatch::hw
{

    //==============================================================================
    /**
     * @class Fan
     *
     * Models a 4-pin PWM fan
     */
    class Fan : public unique_ownership_t<Fan>,
                public io::InputOutput<double>
    {
    public:
        //==============================================================================
        /**
         * @brief Construct Fan object (write-only)
         *
         * @param [in] pwmOut           GPIO connected to fan's PWM input
         * @param [in] maxSpeedRpm      Fan's max speed in RPM
         */
        Fan(io::PWM::Ptr&& pwmOut, double maxSpeedRpm);

        /**
         * @brief Construct Fan object (read + write)
         *
         * @param [in] pwmOut           GPIO connected to fan's PWM input
         * @param [in] tachometerIn     GPIO connected to fan's tachometer output
         * @param [in] maxSpeedRpm      Fan's max speed in RPM
         */
        Fan(io::PWM::Ptr&& pwmOut, io::Tachometer::Ptr&& tachometerIn, double maxSpeedRpm);

        /**
         * @brief Destroy Fan object
         */
        virtual ~Fan();

        //==============================================================================
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

        //==============================================================================
        double getMaxSpeedRpm() { return _maxSpeedRpm; }


    private:
        //==============================================================================
        io::PWM::Ptr _pwm;
        io::Tachometer::Ptr _tachometer;

        //==============================================================================
        double _maxSpeedRpm;
    };

} // namespace beewatch::hw
