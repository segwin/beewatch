//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "util/db.h"

#include "global/logging.h"

namespace beewatch
{
    
    //==============================================================================
    /// Global MongoDB instance
    // TODO: Integrate with mongocxx driver
    using mongocxx_instance_dummy = int;
    using mongocxx_uri_dummy = std::string;
    using mongocxx_client_dummy = std::string;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
    static mongocxx_instance_dummy dbDriver;
#pragma GCC diagnostic pop
    
    //==============================================================================
    struct DB::impl
    {
        mongocxx_client_dummy client;
    };

    //==============================================================================
    DB::DB(std::string name, std::string host, uint16_t port)
        : _name(name), _host(host), _port(port)
    {
        // Create client for given host
        mongocxx_uri_dummy uri(_host + std::to_string(_port));

        pimpl.reset(new impl{ mongocxx_client_dummy(uri) });
    }

    DB::~DB()
    {
        pimpl.reset();
    }

    //==============================================================================
    std::map<int64_t, ClimateData<double>> DB::getClimateData(int64_t since)
    {
        std::unique_lock<std::mutex> lock(_connectionMutex);

        std::map<int64_t, ClimateData<double>> data;


        // TODO: Read data from DB

        return data;
    }

    void DB::addClimateData(int64_t timestamp, ClimateData<double> data)
    {
        std::unique_lock<std::mutex> lock(_connectionMutex);

        // TODO: Add data to DB
    }

    //==============================================================================
    std::string DB::getName()
    {
        std::unique_lock<std::mutex> lock(_connectionMutex);

        std::string name;

        // TODO: Get name from DB
        
        return name;
    }

    void DB::setName(std::string /*name*/)
    {
        std::unique_lock<std::mutex> lock(_connectionMutex);

        // TODO: Update name in DB
    }

} // namespace beewatch
