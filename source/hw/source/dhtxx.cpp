//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "hw/dhtxx.h"

#include <util/assert.h>
#include <util/logging.h>
#include <util/priority.h>

#include <iostream>
#include <thread>
#include <vector>

#include <sys/time.h>

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
            _gpio->setMode(GPIO::Mode::Input);
        }

        DHTxx::~DHTxx()
        {
        }


        //================================================================
        std::vector<struct timespec> g_readTimestamps;

        static void readISRCallback()
        {
            g_readTimestamps.emplace_back();
            clock_gettime(CLOCK_MONOTONIC, &g_readTimestamps.back());
        }

        ClimateData DHTxx::read()
        {
            ClimateData result = { BAD_READ, BAD_READ };

            // Only one read is allowed at a time
            std::lock_guard<std::mutex> lock(_readMutex);

            // Require real-time thread and process priority
            PriorityGuard realtime(Priority::RealTime);

            // 0.0. Configure GPIO ISR routine to update timestamps in local array
            dbgAssert(g_readTimestamps.empty());

            _gpio->setEdgeDetection(GPIO::EdgeType::Both, readISRCallback);

            // 1. Output LO for 25ms (>18ms given in spec sheet)
            //    This causes 2 edge transitions.
            _gpio->setMode(GPIO::Mode::Output);
            _gpio->write(LogicalState::LO);

            std::this_thread::sleep_for(std::chrono::milliseconds(25));

            _gpio->write(LogicalState::HI);
            _gpio->setMode(GPIO::Mode::Input);

            // 2. Wait for 160us: 2 edge transitions (LO ~80us, HI ~80us)
            std::this_thread::sleep_for(std::chrono::microseconds(160));

            // 3. Wait for 5ms (max Tx time): 40 bits with 2 edge transitions each (HI->LO->...)
            std::this_thread::sleep_for(std::chrono::milliseconds(5));

            // 4. Calculate bit values from timestamps
            dbgAssert(g_readTimestamps.size() == (2 + 2 + 80));

            uint8_t bytes[NUM_BYTES_PER_READ];

            for (int i = 0; i < NUM_BITS_PER_READ; ++i)
            {
                using std::chrono::seconds;
                using std::chrono::microseconds;
                using std::chrono::nanoseconds;

                static int idxFirstHiStart = 4 + 1;

                int idxHiStart = idxFirstHiStart + 2*i;
                int idxHiEnd = idxHiStart + 1;

                auto hiTime = seconds(g_readTimestamps[idxHiEnd].tv_sec - g_readTimestamps[idxHiStart].tv_sec) +
                              nanoseconds(g_readTimestamps[idxHiEnd].tv_nsec - g_readTimestamps[idxHiStart].tv_nsec);

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

            for (int i = 0; i < (NUM_BYTES_PER_READ - 1); ++i)
            {
                checksum += bytes[i];
            }

            if (checksum == bytes[NUM_BYTES_PER_READ - 1])
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

            return result;
        }

    } // namespace hw
} // namespace beewatch
