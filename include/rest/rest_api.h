//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "data_types.h"
#include "manager.h"
#include "patterns.h"

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace beewatch::rest
{

    //==============================================================================
    /**
     * @class RestAPI
     */
    class RestAPI : public unique_ownership_t<RestAPI>
    {
    public:
        //==============================================================================
        /**
         * @brief Construct interface around a given port
         *
         * @param [in] port     Port to listen on
         */
        RestAPI(int port, Manager& manager);

        /**
         * @brief Default destructor
         */
        virtual ~RestAPI();
        
        //==============================================================================
        /**
         * @brief Start or restart listener thread
         */
        void restart();

        /**
         * @brief Stop listener thread
         */
        void stop();

        //==============================================================================
        /**
         * @brief Get temperature & humidity samples dated from the given timestamp
         *
         * @param [in] since    Unix timestamp of earliest sample to get
         *
         * @returns Climate data ordered by sample time
         */
        std::map<time_t, ClimateData<double>> getInteriorClimateSamples(time_t since = 0) const;
        std::map<time_t, ClimateData<double>> getExteriorClimateSamples(time_t since = 0) const;

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
         * @param [in] name     New name to use for device
         */
        void setName(std::string name);
        
        //==============================================================================
        /**
         * @brief Get application version
         *
         * @returns Application version string
         */
        std::string getVersion() const;


    private:
        //==============================================================================
        int _port;
        Manager& _manager;
        
        //==============================================================================
        std::unique_ptr<std::thread> _thread;

        void listen();

        //==============================================================================
        std::mutex                   _mutex;
        std::condition_variable      _stopCondition;
        std::atomic<bool>            _changingState;

        void stop(std::unique_lock<std::mutex>& lock);
    };

} // namespace beewatch::rest

