//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <hw/hw_api.h>

#include <io/io.h>
#include <io/gpio.h>

#include <bitset>
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
         * @class DHT
         *
         * Models a DHT sensor, which reports temperature & humidity
         * over a serial interface.
         */
        class HW_API DHTxx : public io::Input<ClimateData>
        {
        public:
            enum class Type
            {
                DHT11 = 11,
                DHT22 = 22
            };

            //================================================================
            /**
             * @brief Sets up a DHT communication on a given GPIO
             *
             * NB: Consumes the given GPIO. It will be released on object destruction.
             *
             * @param [in] dthType  Type of DHT sensor
             * @param [in] gpio     GPIO to use to interface with DHT sensor
             */
            DHTxx(Type dhtType, io::GPIO::Ptr&& gpio);
            
            /**
             * @brief Close DHT communication, releasing owned GPIO
             */
            virtual ~DHTxx();

            //================================================================
            /**
             * Shared pointer to a DHT communication link
             */
            using Ptr = std::shared_ptr<DHTxx>;

            //================================================================
            static constexpr double BAD_READ = 0xFFFFFBAD;

            /**
             * @brief Read current state of DHT sensor
             *
             * Implements the DHT serial communication protocol to read humidity
             * and temperature data from the sensor.
             *
             * @returns Relative humidity and temperature
             */
            virtual ClimateData read() override;

        private:
            //================================================================
            Type _type;
            io::GPIO::Ptr _gpio;

            //================================================================
            static constexpr int READ_BITS = 40;
            static constexpr int READ_BYTES = READ_BITS / 8;

            static constexpr int READ_TIMEOUT_US = 100;

            std::array<uint8_t, READ_BYTES> readData();
            static bool validateData(const std::array<uint8_t, READ_BYTES>& data);

            static inline uint16_t buildU16(uint8_t high8, uint8_t low8)
            {
                return ((uint16_t)high8 << 8) + (uint16_t)low8;
            }
        };

    } // namespace hw
} // namespace beewatch
