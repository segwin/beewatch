//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "hw/dhtxx.h"
#include "hw/hx711.h"
#include "io/gpio.h"
#include "http/server.h"
#include "util/db.h"

#include "util/patterns.hpp"

#include <map>
#include <shared_mutex>
#include <thread>


namespace beewatch
{

    //==============================================================================
    class Manager : public singleton_t<Manager>,
                    public http::IManager
    {
    public:
        //==============================================================================
        /// Interpret command line arguments and initialise application
        void init(int argc, char * argv[]);

        /// Print command line usage message
        void printUsage();
        
        //==============================================================================
        /// Start app
        void start(bool blocking = true);

        //==============================================================================
        /**
         * @brief Get device name
         *
         * @returns Current device name
         */
        virtual std::string getName() const override;
        
        /**
         * @brief Set device name
         *
         * @param [in] name     New device name
         */
        virtual void setName(std::string name) override;
        
        //==============================================================================
        /**
         * @brief Get climate data
         *
         * @param [in] since    Unix timestamp of earliest sample to get
         *
         * @returns Climate data ordered by sample time
         */
        virtual std::map<int64_t, ClimateData<double>> getClimateSamples(std::string sensorID,
                                                                         int64_t since = 0) const override;

        /**
         * @brief Get array of sensor IDs
         *
         * NB: Currently returns a constant value: { "interior", "exterior" }
         *
         * @returns Climate sensor IDs
         */
        virtual std::vector<std::string> getClimateSensorIDs() const override;

        /**
         * @brief Clear all climate data
         *
         * WARNING: This action is irreversible! The data will be cleared from the DB
         */
        virtual void clearClimateData() override;


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
        // Web server
        http::Server::Ptr _apiServer;

        //==============================================================================
        // Database
        DB::Ptr _db;

        //==============================================================================
        // Sensors
        std::map<std::string, hw::DHTxx::Ptr> _climateSensors;
    };

    #define g_manager   Manager::get()

} // namespace beewatch



