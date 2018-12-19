//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "util/data_types.hpp"
#include "util/patterns.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>

namespace beewatch
{

    //==============================================================================
    class DB : public unique_ownership_t<DB>
    {
    public:
        //==============================================================================
        /**
         * @brief Construct a DB client, connecting to instance on the given host
         *
         * If the connection fails (e.g. DB inaccessible), an exception is thrown.
         *
         * @throws std::invalid_argument
         */
        DB(std::string name = DEFAULT_NAME,
           std::string host = DEFAULT_HOST, uint16_t port = DEFAULT_PORT);

        /**
         * @brief Destroy DB object, closing MongoDB connection
         */
        ~DB();


        //==============================================================================
        /// Read climate data from DB
        std::map<time_t, ClimateData<double>> getClimateData(time_t since = 0);

        /// Append climate data to DB
        void addClimateData(time_t timestamp, ClimateData<double> data);


        //==============================================================================
        /// Read device name from DB
        std::string getName();

        /// Update device name in DB
        void setName(std::string name);

        //==============================================================================
        static constexpr const char * DEFAULT_NAME = "beewatch";
        static constexpr const char * DEFAULT_HOST = "localhost";
        static constexpr uint16_t DEFAULT_PORT = 27017;


    private:
        //==============================================================================
        /// MongoDB client implementation (pimpl)
        struct impl;

        std::unique_ptr<impl> pimpl;

        //==============================================================================
        std::mutex _connectionMutex;
        bool _isConnected;

        std::string _name;
        std::string _host;
        uint16_t _port;
    };

} // namespace beewatch::db
