//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <hw/hw_api.h>

#include <io/io.h>
#include <io/gpio.h>

#include <memory>

namespace beewatch
{
    namespace hw
    {

        //================================================================
        struct ClimateData
        {
            double humidity;
            double temperature;
        };

        //================================================================
        /**
         * @class DHTxx
         *
         * Models a DHT11/22 sensor, which reports temperature & humidity
         * over an I2C interface.
         */
        class HW_API DHTxx : public io::Input<ClimateData>
        {
        public:
            //================================================================
            /**
             * @brief Sets up a DHT11/22 communication on a given GPIO
             *
             * NB: Consumes the given GPIO. It will be released on object destruction.
             *
             * @param [in] gpio                 GPIO to use for PWM
             */
            DHTxx(io::GPIO::Ptr&& gpio);
            
            /**
             * @brief Close DHT11/22 communication, releasing owned GPIO
             */
            virtual ~DHTxx();

            //================================================================
            /**
             * Shared pointer to a DHT11/22 communication link
             */
            using Ptr = std::shared_ptr<DHTxx>;

            //================================================================
            static constexpr double BAD_READ = 0xFFFFFBAD;

            /**
             * @brief Read current state of DHT11/22 sensor
             *
             * Implements the DHT11/22 1-wire communication protocol to read
             * humidity and temperature data from the sensor.
             *
             * @returns Relative humidity and temperature
             */
            virtual ClimateData read() override;

        private:
            //================================================================
            io::GPIO::Ptr _gpio;

            //================================================================
            static constexpr int NUM_BITS_PER_READ = 40;
            static constexpr int NUM_BYTES_PER_READ = NUM_BITS_PER_READ / 8;
        };

    } // namespace hw
} // namespace beewatch
