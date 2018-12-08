//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "util/data_types.hpp"
#include "util/patterns.hpp"

#include <atomic>
#include <condition_variable>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace beewatch::http
{
    
    //==============================================================================
    /**
     * @interface IManager
     *
     * HTTP server's interface for the manager class
     */
    class IManager
    {
    public:
        //==============================================================================
        virtual std::string getName() const = 0;
        virtual void setName(std::string name) = 0;
        
        //==============================================================================
        virtual std::map<time_t, ClimateData<double>> getClimateSamples(time_t since = 0) const = 0;
    };

    //==============================================================================
    /**
     * @class Server
     */
    class Server : public unique_ownership_t<Server>
    {
    public:
        //==============================================================================
        /**
         * @brief Construct interface around a given port
         *
         * @param [in] port     Port to listen on
         */
        Server(int port, std::string webRoot, IManager& manager);

        /**
         * @brief Default destructor
         */
        virtual ~Server();
        
        //==============================================================================
        void setWebRoot(std::string webRootPath);
        
        //==============================================================================
        /**
         * @brief Start or restart listener thread
         */
        void restart();

        /**
         * @brief Stop listener thread
         */
        void stop();


    private:
        //==============================================================================
        int _port;

        std::string _webRoot;
        std::vector<std::string> _webResources;

        IManager& _manager;
        
        //==============================================================================
        std::unique_ptr<std::thread> _thread;

        void listen();

        //==============================================================================
        std::mutex                   _mutex;
        std::condition_variable      _stopCondition;
        std::atomic<bool>            _changingState;

        void stop(std::unique_lock<std::mutex>& lock);
    };

} // namespace beewatch::http
