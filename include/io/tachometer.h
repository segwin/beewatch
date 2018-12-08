//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "io.h"
#include "gpio.h"

#include "util/patterns.hpp"

#include <atomic>
#include <thread>

namespace beewatch::io
{

    //==============================================================================
    /**
     * @class Tachometer
     *
     * Models a tachometer interface
     */
    class Tachometer : public unique_ownership_t<Tachometer>,
                       public Input<double>
    {
    public:
        //==============================================================================
        /**
         * @brief Construct a Tachometer object on a given GPIO
         *
         * NB: Consumes the given GPIO. It will be released on object destruction.
         *
         * @param [in] gpio                 GPIO to use for PWM
         * @param [in] pulsesPerRevolution  Number of pulses counted per revolution
         */
        Tachometer(GPIO::Ptr&& gpio, int pulsesPerRevolution = 2);
        
        /**
         * @brief Destroy Tachometer object, releasing owned GPIO
         */
        virtual ~Tachometer();

        //==============================================================================
        static constexpr double c_readTimeMs = 500.0;

        virtual double read() override;

        
    private:
        //==============================================================================
        GPIO::Ptr _gpio;

        //==============================================================================
        const int _pulsesPerRevolution;

        double _speedRpm;
    };

} // namespace beewatch
