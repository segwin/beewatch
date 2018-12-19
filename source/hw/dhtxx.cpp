//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "hw/dhtxx.h"

#include "global/logging.h"
#include "util/priority.h"
#include "global/time.hpp"

#include <thread>

namespace beewatch::hw
{

    using io::GPIO;
    using io::LogicalState;

    //==============================================================================
    DHTxx::DHTxx(Type dhtType, GPIO::Ptr&& gpio)
        : _type(dhtType)
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

    DHTxx::~DHTxx() = default;

    //==============================================================================
    static constexpr std::array<uint8_t, 5> readError { {0xFF, 0xFF, 0xFF, 0xFF, 0xFF} };

    DHTxx::Data DHTxx::read()
    {
        Data result;

        // Only one read is allowed at a time
        std::lock_guard<std::mutex> lock(_readMutex);

        // Require real-time thread and process priority
        PriorityGuard realtime(Priority::RealTime);

        int i = 0;
        while (i++ < 10)
        {
            auto data = readData();

            if ( std::equal(data.begin(), data.end(), readError.begin()) )
            {
                g_logger.print(Logger::Warning, "DHT read failed: timeout reached");

                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            if (!validateData(data))
            {
                g_logger.print(Logger::Warning, "DHT read failed: bad checksum");

                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            
            // Calculate floating-point values for measurements
            switch (_type)
            {
            case Type::DHT11:
                {
                    result.humidity = data[0];
                    result.temperature = data[2];
                }
                break;

            case Type::DHT22:
            default:
                {
                    result.humidity = 0.1 * (double)buildU16(data[0], data[1]);
                    result.temperature = 0.1 * (double)buildU16(data[2] & 0x7F, data[3]);

                    if (data[2] & 0x80)
                    {
                        // Negative temperature
                        result.temperature *= -1;
                    }
                }
                break;
            }

            return result;
        }

        g_logger.print(Logger::Error, "Failed to read data from DHT sensor: number of attempts exceeded");
        return result;
    }

    std::array<uint8_t, DHTxx::READ_BYTES> DHTxx::readData()
    {
        /**
         * 0. Allocate memory for data and timestamps
         */
        std::array<uint8_t, READ_BYTES> bytes;
        bytes.fill(0);

        uint16_t byte = 0;
        uint16_t mask = 0b1000'0000;

        double startMs;
        double nowMs;
        double diffMs;


        /**
         * 1. Output LO for 20ms (>18ms given in spec sheet)
         */
        _gpio->setMode(GPIO::Mode::Output);

        _gpio->write(LogicalState::LO);
        g_timeRaw.wait(20.0);

        _gpio->write(LogicalState::HI);
        g_timeRaw.wait(40e-3);

        _gpio->setMode(GPIO::Mode::Input);


        /**
         * 2. DHT should signal LO (80us), then HI (80us)
         */
        // LO
        startMs = g_timeRaw.now();

        do
        {
            nowMs = g_timeRaw.now();
            diffMs = nowMs - startMs;

            if (diffMs > READ_TIMEOUT_MS)
            {
                return readError;
            }
        } while (_gpio->read() == LogicalState::LO);

        // LO
        startMs = g_timeRaw.now();

        do
        {
            nowMs = g_timeRaw.now();
            diffMs = nowMs - startMs;

            if (diffMs > READ_TIMEOUT_MS)
            {
                return readError;
            }
        } while (_gpio->read() == LogicalState::HI);

        /**
         * 3. DHT will send 40 bits via LO (50us), then HI (26-28us for 0, 70us for 1)
         */
        for (int i = 0; i < READ_BITS; ++i)
        { 
            // LO
            startMs = g_timeRaw.now();

            do
            {
                nowMs = g_timeRaw.now();
                diffMs = nowMs - startMs;

                if (diffMs > READ_TIMEOUT_MS)
                {
                    return readError;
                }
            } while (_gpio->read() == LogicalState::LO);

            // HI
            startMs = g_timeRaw.now();

            do
            {
                nowMs = g_timeRaw.now();
                diffMs = nowMs - startMs;

                if (diffMs > READ_TIMEOUT_MS)
                {
                    return readError;
                }
            } while (_gpio->read() == LogicalState::HI);
            
            // Mid-point between bit value times is 48.5
            if (diffMs > 48.5e-3)
            {
                bytes[byte] |= mask;
            }

            mask /= 2;
            if (mask == 0)
            {
                mask = 0b1000'0000;
                byte++;
            }
        }

        return bytes;
    }

    bool DHTxx::validateData(const std::array<uint8_t, READ_BYTES>& data)
    {
        uint8_t checksum = 0;

        for (size_t i = 0; i < data.size() - 1; ++i)
        {
            checksum += data[i];
        }

        return checksum == data[data.size() - 1];
    }

} // namespace beewatch::hw
