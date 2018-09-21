//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include "hw/dhtxx.h"
#include "hw/fan.h"

#include <memory>
#include <mutex>
#include <thread>
#include <vector>

namespace beewatch
{
    namespace ctrl
    {

        //================================================================
        /**
         * @class ClimateCtrl
         *
         * Implements a control system for the shelter's climate using a
         * temperature/humidity sensor as input and a fan as output.
         */
        class ClimateCtrl
        {
        public:
            //================================================================
            /**
             * @brief Constructs the climate control system around a sensor and a fan
             *
             * NB: Consumes the given fan object. It will be released on destruction.
             *
             * @param [in] sensor   Temperature & humidity sensor
             * @param [in] fan      Fan to control
             */
            ClimateCtrl(std::vector<hw::DHTxx::Ptr> sensors, hw::Fan::Ptr fan);
            
            /**
             * @brief Destroy climate control system, releasing owned fan object
             */
            virtual ~ClimateCtrl();


            //================================================================
            /**
             * Unique pointer to a climate control object
             */
            using Ptr = std::unique_ptr<ClimateCtrl>;


            //================================================================
            /**
             * @brief Start climate control system
             */
            void start();

            /**
             * @brief Stop climate control system
             */
            void stop();


            //================================================================
            static constexpr Range<double> temperatureRange = { 2.5, 7.5 };
            static constexpr double temperatureTarget = temperatureRange.mid();

            static constexpr Range<double> humidityRange = { 0.50, 0.75 };
            static constexpr double humidityTarget = humidityRange.mid();


        private:
            //================================================================
            std::vector<hw::DHTxx::Ptr> _sensors;
            hw::Fan::Ptr _fan;
            

            //================================================================
            mutable std::mutex _climateDataMutex;
            hw::DHTxx::Data _climateData;

            /**
             * @brief Poll sensors for climate data
             *
             * Called internally by ctrlLoop() method
             */
            inline void getClimateData();

            /**
             * @brief Update fan speed based on climate data
             *
             * Target ranges:
             *     - Temperature: 2.5deg to 7.5deg (Celsius)
             *     - Humidity: 50% to 75%
             */
            inline void setFanSpeed();

            //================================================================
            bool _isActive = false;

            bool _stopRequested = false;
            std::unique_ptr<std::thread> _ctrlThread;
            std::mutex _ctrlMutex;

            /**
             * @brief Climate control loop
             *
             * Polls climate data from sensors and updates the fan speed in response
             */
            void ctrlLoop();


        };

    } // namespace ctrl
} // namespace beewatch
