//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "util/db.h"

#include "global/logging.h"

// TODO: Modify file to make sure it compiles even without mongocxx (for CI)
#ifdef HAS_MONGOCXX
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>
#endif

namespace beewatch
{

    //==============================================================================
    /// Global MongoDB driver instance
    static mongocxx::instance g_dbDriver{};

    //==============================================================================
    struct DB::impl
    {
        //==============================================================================
        /// Construct MongoDB client & get database
        impl(std::string uri, std::string name)
        {
            // Build client
            client = mongocxx::client( mongocxx::uri(uri) );

            // Get database from client
            db = client[name];
        }

        //==============================================================================
        /// MongoDB database
        mongocxx::database db;

    private:
        //==============================================================================
        /// MongoDB client
        mongocxx::client client;
    };

    //==============================================================================
    DB::DB(std::string name, std::string host, uint16_t port)
        : _name(name), _host(host), _port(port)
    {
        // Create client for given host & database
        std::string uri = "mongodb://" + _host + ":" + std::to_string(_port);

        pimpl = std::make_unique<impl>(uri, _name);
    }

    DB::~DB()
    {
        pimpl.reset();
    }

    //==============================================================================
    std::map<int64_t, ClimateData<double>> DB::getClimateData(std::string sensorID, int64_t since)
    {
        std::unique_lock<std::mutex> lock(_connectionMutex);

        std::map<int64_t, ClimateData<double>> data;

        // Find all data in "climate_data" collection with timestamps >= since
        using document_stream = bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::open_document;
        using bsoncxx::builder::stream::close_document;
        using bsoncxx::builder::stream::finalize;

        mongocxx::collection collection = pimpl->db["climate_data"];
        mongocxx::cursor results = collection.find(             // Filter documents:
            document_stream{} << "sensor_id"                    //   - by sensor ID
                                << sensorID
                              << "time"                         //   - by timestamp (>= since)
                                << open_document
                                  << "$gte" << since
                                << close_document
                              << finalize
        );

        for (auto document : results)
        {
            // Get sample data from document
            ClimateData<double> sample;

            sample.temperature = document["temperature"].get_double();
            sample.humidity = document["humidity"].get_double();

            int64_t timestamp = document["time"].get_int64();

            // Save sample
            data[timestamp] = sample;
        }

        return data;
    }

    void DB::addClimateData(std::string sensorID, int64_t timestamp, ClimateData<double> data)
    {
        std::unique_lock<std::mutex> lock(_connectionMutex);

        // Build document for given data
        using document_stream = bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::open_document;
        using bsoncxx::builder::stream::close_document;
        using bsoncxx::builder::stream::finalize;

        auto document = document_stream{} << "sensor_id" << sensorID
                                          << "time" << timestamp
                                          << "temperature" << data.temperature
                                          << "humidity" << data.humidity
                                          << finalize;

        // Add document to DB's "climate_data" collection
        auto collection = pimpl->db["climate_data"];
        collection.insert_one(document.view());
    }

    //==============================================================================
    std::string DB::getName()
    {
        std::unique_lock<std::mutex> lock(_connectionMutex);

        // Get document in "meta" collection
        auto collection = pimpl->db["meta"];
        auto maybeDocument = collection.find_one({});

        if (maybeDocument)
        {
            auto document = maybeDocument->view();
            auto nameElement = document["name"];

            if (nameElement.type() == bsoncxx::type::k_utf8)
            {
                // Only return name if it exists in the document as a string
                return nameElement.get_utf8().value.data();
            }
        }

        return "";
    }

    void DB::setName(std::string name)
    {
        std::unique_lock<std::mutex> lock(_connectionMutex);

        // Try to update document in "meta" collection
        using document_stream = bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::open_document;
        using bsoncxx::builder::stream::close_document;
        using bsoncxx::builder::stream::finalize;

        auto collection = pimpl->db["meta"];
        auto maybeDocument = collection.update_one(
            {}, // Any document
            document_stream{} << "$set"
                                << open_document
                                  << "name" << name
                                << close_document
                              << finalize
        );

        // If document was not updated (e.g. no document exists), create a new one
        if (!maybeDocument)
        {
            auto newDocument = document_stream{} << "name" << name
                                                 << finalize;

            collection.insert_one(newDocument.view());
        }
    }

} // namespace beewatch
