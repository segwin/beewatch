//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "hw/dhtxx.h"

#include <io/pigpio.h>
#include <util/logging.h>
#include <util/priority.h>

#include <iostream>

namespace beewatch
{
    namespace hw
    {

        using io::GPIO;
        using io::LogicalState;

        //================================================================
        DHTxx::DHTxx(GPIO::Ptr&& gpio)
        {
            if (!gpio)
            {
                throw std::invalid_argument("Received undefined GPIO");
            }

            // Take ownership of GPIO
            _gpio = std::move(gpio);

            // Configure GPIO
            io::PiGPIOLib::init();

            _gpio->setDirection(GPIO::In);
        }

        DHTxx::~DHTxx()
        {
        }


        //================================================================
        ClimateData DHTxx::read()
        {
            using namespace std::chrono;

            // Only one read is allowed at a time
            std::lock_guard<std::mutex> lock(_readMutex);

            // Allocate memory for time point & binary arrays
            time_point<high_resolution_clock> startTimes[NUM_BITS_PER_READ];
            time_point<high_resolution_clock> endTimes[NUM_BITS_PER_READ];

            uint8_t bytes[NUM_BYTES_PER_READ];

            ClimateData result = { BAD_READ, BAD_READ };

            // Require real-time thread and process priority
            util::setPriority(util::Priority::REALTIME);

            // 1. Output LO for 25ms (>18ms given in spec sheet)
            _gpio->setDirection(GPIO::Out);
            _gpio->write(LogicalState::LO);

            std::this_thread::sleep_for(std::chrono::milliseconds(25));

            _gpio->write(LogicalState::HI);
            _gpio->setDirection(GPIO::In);

            // 2. Wait for sensor to signal LO (~80us), followed by HI (~80us)
            _gpio->waitForState(LogicalState::LO);
            _gpio->waitForState(LogicalState::HI);

            // 3. Receive sensor data
            _gpio->waitForState(LogicalState::LO);
                
            for (int i = 0; i < NUM_BITS_PER_READ; ++i)
            {
                // HI for 26-28us if 0, 70us if 1
                _gpio->waitForState(LogicalState::HI);
                startTimes[i] = high_resolution_clock::now();

                // LO for 50us
                _gpio->waitForState(LogicalState::LO);
                endTimes[i] = high_resolution_clock::now();
            }

            // 4. Calculate bit values
            for (int i = 0; i < NUM_BITS_PER_READ; ++i)
            {
                auto hiTime = endTimes[i] - startTimes[i];

                // Mid-point between bit value times is 48.5
                int byte = i / 8;
                int offset = i % 8;

                if (hiTime > microseconds(48))
                {
                    bytes[byte] |= 1 << offset;
                }
                else
                {
                    bytes[byte] &= ~(1 << offset);
                }
            }

            // 5. Calculate and verify checksum
            uint8_t checksum = 0;

            for (int i = 0; i < 4; ++i)
            {
                checksum += bytes[i];
            }

            if (checksum == bytes[5])
            {
                // 6. Calculate floating-point values for measurements
                uint16_t humidityH = ((uint16_t)bytes[0]) << 8;
                uint16_t humidityL = bytes[1];

                result.humidity = (humidityH + humidityL) * 0.1;

                uint16_t temperatureH = ((uint16_t)bytes[2] & 0x7f) << 8;
                uint16_t temperatureL = bytes[3];

                result.temperature = (temperatureH + temperatureL) * 0.1;

                if (bytes[2] & 0x80)
                {
                    // Negative temperature
                    result.temperature *= -1;
                }
            }
            else
            {
                logger.dualPrint(Logger::Error, "DHT read failed (bad checksum)");
            }

            // Reset to normal priority
            util::setPriority(util::Priority::NORMAL);

            return result;
        }

    } // namespace hw
} // namespace beewatch
