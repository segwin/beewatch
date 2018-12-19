//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "util/data_types.hpp"
#include "util/patterns.hpp"

#include <cstdint>
#include <map>
#include <shared_mutex>

namespace beewatch
{

    //==============================================================================
    class DB : public singleton_t<DB>
    {
    public:
        //==============================================================================
        /**
         * @brief Destroy DB object, running disconnect() if necessary
         */
        ~DB();


        //==============================================================================
        /**
         * @brief Connect to the given MongoDB instance
         *
         * @param [in] host     Host on which DB is running
         * @param [in] port     Port to use to connect to DB
         *
         * @throws std::runtime_error if connection fails
         */
        void connect(std::string host = "localhost", uint16_t port = 27017);

        /**
         * @brief Disconnect from the current MongoDB instance, if any
         */
        void disconnect();

        /**
         * @brief Check current connection status
         *
         * @returns True if a DB connection is active, false otherwise
         */
        bool isConnected() const;


        //==============================================================================
        /// Read climate data from DB
        std::map<time_t, ClimateData<double>> getClimateData(time_t since = 0) const;

        /// Append climate data to DB
        void addClimateData(time_t timestamp, ClimateData<double> data);


        //==============================================================================
        /// Read device name from DB
        std::string getName() const;

        /// Update device name in DB
        void setName(std::string name);


    protected:
        //==============================================================================
        /**
         * @brief Construct a DB object
         *
         * Tries to connect to the local MongoDB instance on the default port. Callers
         * should check the connection status with DB::IsConnected() before using the DB
         * as the connection attempt may fail.
         */
        DB();


    private:
        //==============================================================================
        std::shared_mutex _connectionMutex;
        bool _isConnected;

        std::string _host;
        uint16_t _port;
    };

    #define g_db    DB::get()

} // namespace beewatch::db
