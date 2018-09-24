//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "io/gpio.h"

#include <memory>

namespace beewatch
{
    namespace hw
    {

        //==============================================================================
        /**
         * @class HX711
         *
         * Models a HX711 ADC for load cell measurements
         */
        class HX711 : public io::Input<double>
        {
        public:
            //==============================================================================
            /**
             * @brief Sets up communication with an HX711 amplifier
             *
             * NB: Consumes the given GPIOs. They will be released on object
             * destruction.
             *
             * @param [in] doutPin  Pin over which to receive serial data
             * @param [in] pdSckPin Pin over which to send clock & power signal
             */
            HX711(io::GPIO::Ptr&& doutPin, io::GPIO::Ptr&& pdSckPin);

            /**
             * @brief Destroy HX711 object
             */
            virtual ~HX711();
            
            
            //==============================================================================
            /**
             * Unique pointer to an HX711 object
             */
            using Ptr = std::unique_ptr<HX711>;


            //==============================================================================
            virtual double read() override;


        protected:
            //==============================================================================
            static constexpr auto NUM_SAMPLES_PER_READ = 8;


            //==============================================================================
            /**
             * @brief Enable HX711 amplifier
             */
            void enable();

            /**
             * @brief Disable HX711 amplifier
             */
            void disable();
            
            /**
             * @brief Calibrate scale to obtain scale factor & tare weight
             */
            void calibrate();

            /**
             * @brief Tells us whether we can safely interact with the HX711
             *
             * @returns True if communication is established, false otherwise
             */
            bool isReady();

            /**
             * @brief Tells us whether object is correctly configured
             *
             * @returns False if GPIOs are not owned or if HX711 is disabled
             */
            bool isValid();
            
            
            //==============================================================================
            /**
             * @brief Shift in one byte
             *
             * Sends a clock pulse to read the next byte over DOUT
             */
            inline uint8_t shiftIn();

            /**
             * @brief Read raw output from HX711
             *
             * @returns 24-bit signed integer value
             */
            int32_t readRaw();

            /**
             * @brief Read an average of the HX711's raw output
             *
             * @param [in] numSamples   Number of reads to average into result
             *
             * @returns 24-bit signed integer value
             */
            int32_t readRawAvg(size_t numSamples);


            //==============================================================================
            bool _isOn;             // Current power state
            int _gainSetting;       // Amplifier gain factor

            int _tare;              // Tare (unladen) weight
            double _kgPerVolt;      // Scale factor obtained through calibration

            io::GPIO::Ptr _dout;    // Serial data from HX711
            io::GPIO::Ptr _pdSck;   // Power down & serial clock to HX711
        };

    } // namespace hw
} // namespace beewatch
