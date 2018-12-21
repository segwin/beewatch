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
         * @brief Destroy DB object, closing DB connection
         */
        ~DB();


        //==============================================================================
        /// Read climate data from DB
        std::map<int64_t, ClimateData<double>> getClimateData(std::string sensorID, int64_t since = 0);

        /// Append climate data to DB
        void addClimateData(std::string sensorID, int64_t timestamp, ClimateData<double> data);


        //==============================================================================
        /// Read device name from DB
        std::string getName();

        /// Update device name in DB
        void setName(std::string name);

        //==============================================================================
        static constexpr auto DEFAULT_NAME = "beewatch";
        static constexpr auto DEFAULT_HOST = "localhost";
        static constexpr uint16_t DEFAULT_PORT = 27017;


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

        std::unique_ptr<impl> pimpl;

        //==============================================================================
        bool _isConnected;

        std::string _name;
        std::string _host;
        uint16_t _port;
    };

} // namespace beewatch::db
