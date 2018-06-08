//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "hw/hx711.h"

#include <util/assert.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

namespace beewatch
{
    namespace hw
    {

        //================================================================
        HX711::HX711(io::GPIO::Ptr&& doutPin, io::GPIO::Ptr&& pdSckPin)
            : _isOn(false), _gainSetting(1), _tare(0), _kgPerVolt(1.0)
        {
            if (!doutPin || !pdSckPin)
            {
                throw std::invalid_argument("Received undefined GPIO");
            }

            // Take ownership of GPIO
            _dout = std::move(doutPin);
            _pdSck = std::move(pdSckPin);

            // Configure GPIO
            _dout->setMode(io::GPIO::Mode::Input);
            _pdSck->setMode(io::GPIO::Mode::Output);

            // Initialise HX711 and run calibration procedure
            enable();

            calibrate();

            disable();
        }

        HX711::~HX711()
        {
            disable();
        }
        
        
        //================================================================
        double HX711::read()
        {
            // Save energy by keeping HX711 powered off except during reads
            enable();

            int32_t avgVal = readRawAvg(NUM_SAMPLES_PER_READ);

            disable();

            // Calculate mass from acquired value and calibration factors
            return (double)(avgVal - _tare) * _kgPerVolt;
        }

        int32_t HX711::readRaw()
        {
            dbgAssert(isValid());

            std::lock_guard<std::mutex> lock(_readMutex);
            
            uint32_t val;
            uint8_t data[3];

            // Wait for ready state
            while (!isReady())
            {
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }

            // Read data (24 bit integer)
            for (int i = 0; i < 3; ++i)
            {
                data[i] = shiftIn();
            }

            // Signal gain & channel settings to HX711 by pulsing clock
            //     25 total pulses: Channel A, gain x128
            //     26 total pulses: Channel B, gain x32
            //     27 total pulses: Channel A, gain x64
            for (int i = 0; i < _gainSetting; i++) {
                _pdSck->write(io::LogicalState::HI);
                _pdSck->write(io::LogicalState::LO);
            }

            // Flip bits in 2's complement data
            data[2] = ~data[2];
            data[1] = ~data[1];
            data[0] = ~data[0];

            // Determine sign of data and concatenate data
            if ((data[2] & 0x80)  ||  ((data[2] == 0x7F) && (data[1] == 0xFF) && (data[0] == 0xFF)))
                val = (uint32_t)0xFF<<24;   // negative
            else
                val = (uint32_t)0x00<<24;   // positive

            val |= data[2]<<16 | data[1]<<8 | data[0];

            return (int32_t)val;
        }

        int32_t HX711::readRawAvg(size_t numSamples)
        {
            // Sample HX711 output N times and average results
            int64_t samplesSum = 0;

            for (int i = 0; i < NUM_SAMPLES_PER_READ; ++i)
            {
                samplesSum += readRaw();
            }

            int32_t avgVal = (int32_t)((double)samplesSum / (double)numSamples);

            dbgAssert(avgVal > (int32_t)0xFF800000 && avgVal < (int32_t)0x007FFFFF);

            return avgVal;
        }

        
        //================================================================
        void HX711::enable()
        {
            std::lock_guard<std::mutex> lock(_readMutex);
            _pdSck->write(io::LogicalState::LO);

            _isOn = true;
        }

        void HX711::disable()
        {
            std::lock_guard<std::mutex> lock(_readMutex);
            _pdSck->write(io::LogicalState::HI);

            _isOn = false;
        }
        
        bool HX711::isReady()
        {
            return _dout->read() == io::LogicalState::LO;
        }

        bool HX711::isValid()
        {
            return _dout && _pdSck && _isOn;
        }

        void HX711::calibrate()
        {
            // TODO: Replace console IO with graphical UI (web page?)
            std::cout << "********************************" << std::endl
                      << "BEGINNING SCALE CALIBRATION" << std::endl
                      << "********************************" << std::endl;

            std::cout << std::endl;

            /// Run read cycle to configure HX711 channel & gain factor
            readRaw();

            /// Measure tare
            std::string dummy;

            std::cout << "Remove all weight from the sensor, then press <ENTER> to continue. ";
            std::cin >> dummy;
            std::cin.clear();

            std::cout << "  -> Measuring tare..." << std::endl;

            _tare = readRawAvg(NUM_SAMPLES_PER_READ);

            std::cout << "  -> Tare value = " << _tare << " [V]" << std::endl;

            std::cout << std::endl;

            /// Measure scale factor
            std::cout << "Place a known weight on the sensor, then press <ENTER> to continue. ";
            std::cin >> dummy;
            std::cin.clear();

            double testMassKg;
            while (true)
            {
                std::cout << "  -> Please enter the test weight's mass (kg) and press <ENTER>: ";

                if (std::cin >> testMassKg)
                {
                    break;
                }
                else
                {
                    std::cout << "    ** Invalid input! Value must be a valid fractional or whole number." << std::endl;

                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                }
            }

            std::cout << "  -> Test mass = " << testMassKg << " [kg]" << std::endl;

            int32_t val = readRawAvg(NUM_SAMPLES_PER_READ);
            std::cout << "  -> Measured value = " << val << " [V]" << std::endl;

            _kgPerVolt = testMassKg / (double)(val - _tare);
            std::cout << "  -> Resulting scale factor = " << _kgPerVolt << " [kg/V]" << std::endl;
        }

        
        //================================================================
        inline uint8_t HX711::shiftIn()
        {
            using std::chrono::high_resolution_clock;
            using std::chrono::microseconds;

            dbgAssert(isReady());

            // Shift in 1 byte
            uint8_t result = 0;

            for (int i = 0; i < 8; ++i)
            {
                std::this_thread::sleep_until(high_resolution_clock::now() + microseconds(1));
                _pdSck->write(io::LogicalState::HI);
                std::this_thread::sleep_until(high_resolution_clock::now() + microseconds(1));

                result |= (uint8_t)_dout->read() << (7 - i);
                
                _pdSck->write(io::LogicalState::LO);
            }

            return result;
        }


    } // namespace hw
} // namespace beewatch
