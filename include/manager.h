//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "hw/dhtxx.h"
#include "hw/hx711.h"
#include "io/gpio.h"

#include "logging.h"
#include "patterns.h"
#include "str.h"
#include "timing.h"

#include <chrono>
#include <iostream>
#include <map>
#include <shared_mutex>
#include <thread>
#include <vector>


namespace beewatch
{

    //==============================================================================
    class Manager : public singleton_t<Manager>
    {
    public:
        //==============================================================================
        /**
         * @brief Get device name
         *
         * @returns Current device name
         */
        std::string getName() const;
        
        /**
         * @brief Set device name
         *
         * @param [in] name     New device name
         */
        void setName(std::string name);
        
        //==============================================================================
        /**
         * @brief Get climate data
         *
         * @param [in] since    Unix timestamp of earliest sample to get
         *
         * @returns Climate data ordered by sample time
         */
        std::map<time_t, ClimateData<double>> getClimateSamples(time_t since = 0) const;


    protected:
        //==============================================================================
        /**
         * @brief Manager constructor (accessible only to singleton constructor)
         */
        Manager();


    private:
        //==============================================================================
        std::unique_ptr<std::thread> _ctrlThread;

        /**
         * @brief Main control loop
         */
        void ctrlLoop();

        //==============================================================================
        mutable std::shared_mutex _attrMutex;

        // Attributes
        std::string _name;

        //==============================================================================
        hw::DHTxx::Ptr _climateSensor;
        hw::HX711::Ptr _massSensor;
    };

} // namespace beewatch



