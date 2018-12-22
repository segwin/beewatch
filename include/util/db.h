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

        ~DB();


        //==============================================================================
        /// Drop all tables from DB
        void clear();


        //==============================================================================
        /// Read climate data from DB
        std::map<int64_t, ClimateData<double>> getClimateData(std::string sensorID, int64_t since = 0);

        /// Append climate data to DB
        void addClimateData(std::string sensorID, int64_t timestamp, ClimateData<double> data);

        /// Drop climate data table from DB
        void clearClimateData();


        //==============================================================================
        /// Read device name from DB
        std::string getName();

        /// Update device name in DB
        void setName(std::string name);

        /// Drop About table from DB
        void clearAboutData();

        
        //==============================================================================
        static constexpr auto DEFAULT_NAME = "beewatch";
        static constexpr auto DEFAULT_HOST = "127.0.0.1";
        static constexpr uint16_t DEFAULT_PORT = 5432;


    private:
        //==============================================================================
        /// Check if table exists in DB
        bool hasTable(std::string name);

        /// Create "ClimateData" table
        void createClimateDataTable();

        /// Create "About" table
        void createAboutTable();


        //==============================================================================
        /// DB client implementation (pimpl)
        struct impl;
        impl * pimpl;


        //==============================================================================
        bool _isConnected;

        std::string _name;
        std::string _host;
        uint16_t _port;
    };

} // namespace beewatch::db
