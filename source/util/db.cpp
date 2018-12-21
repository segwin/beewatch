//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "util/db.h"

#include "global/logging.h"
#include "version.h"

#include <pqxx/pqxx> 

namespace beewatch
{

    //==============================================================================
    struct DB::impl : public unique_ownership_t<impl>
    {
        //==============================================================================
        /// Construct PostgreSQL client
        /// TODO: Allow configuration of DB user/password
        impl(std::string host, std::string port, std::string name,
             std::string username = "postgres", std::string password = "postgres")
            : _db("dbname = " + name + " " +
                  "user = " + username + " password = " + password + " " +
                  "hostaddr = " + host + " port = " + port)
        {
        }

        //==============================================================================
        /// Execute query in a single transaction and return results
        pqxx::result exec(std::string query, bool commit)
        {
            std::unique_lock<std::mutex> lock(_mutex);

            pqxx::result results;

            try
            {
                pqxx::work txn(_db);
                results = txn.exec(query);

                if (commit)
                {
                    txn.commit();
                }
            }
            catch (std::exception& e)
            {
                g_logger.error("Caught exception while executing DB transaction: " +
                               std::string(e.what()));
            }

            return results;
        }

    private:
        //==============================================================================
        /// PostgreSQL database connection
        pqxx::connection _db;

        /// Transaction mutex
        std::mutex _mutex;
    };

    //==============================================================================
    DB::DB(std::string name, std::string host, uint16_t port)
        : _name(name), _host(host), _port(port)
    {
        // Create client for given host & database
        pimpl = new impl(_host, std::to_string(port), _name);
    }

    DB::~DB()
    {
        delete pimpl;
    }
    
    //==============================================================================
    bool DB::hasTable(std::string name)
    {
        // Query information table for given table name
        auto query = "SELECT EXISTS ("
                     "  SELECT 1"
                     "  FROM   pg_tables"
                     "  WHERE  tablename = '" + name + "'"
                     ");";

        auto results = pimpl->exec(query, false);

        return !results.empty();
    }

    void DB::createClimateDataTable()
    {
        // Create ClimateData table with unqualified schema
        auto query = "CREATE TABLE ClimateData ("
                     "  rowguid         SERIAL          PRIMARY KEY     NOT NULL,"
                     "  SensorID        VARCHAR(256)                    NOT NULL,"
                     "  Time            BIGINT                          NOT NULL,"
                     "  Temperature     NUMERIC                         NOT NULL,"
                     "  Humidity        NUMERIC                         NOT NULL"
                     ");";

        pimpl->exec(query, true);
    }

    void DB::createAboutTable()
    {
        // Create About table with unqualified schema
        auto query = "CREATE TABLE About ("
                     "  rowguid         SERIAL          PRIMARY KEY     NOT NULL,"
                     "  Name            VARCHAR(256)                    NOT NULL"
                     ");";

        pimpl->exec(query, true);
    }

    //==============================================================================
    std::map<int64_t, ClimateData<double>> DB::getClimateData(std::string sensorID, int64_t since)
    {
        // If ClimateData table does not exist, create it and return (no results stored yet)
        if (!hasTable("ClimateData"))
        {
            createClimateDataTable();
            return {};
        }

        // Find all data in "ClimateData" table with timestamps >= since
        auto query = "SELECT Time, Temperature, Humidity "
                     "FROM ClimateData "
                     "WHERE SensorID = '" + sensorID + "'"
                     "AND Time >= " + std::to_string(since);

        auto results = pimpl->exec(query, false);

        // Get sample data from results
        std::map<int64_t, ClimateData<double>> data;

        for (auto row : results)
        {
            int64_t timestamp = row[0].as<int64_t>();

            data[timestamp].temperature = row[1].as<double>();
            data[timestamp].humidity = row[2].as<double>();
        }

        return data;
    }

    void DB::addClimateData(std::string sensorID, int64_t timestamp, ClimateData<double> data)
    {
        // If ClimateData table does not exist, create it and add data
        if (!hasTable("ClimateData"))
        {
            createClimateDataTable();
        }

        // Build document for given data
        auto query = "INSERT INTO ClimateData ("
                     "  SensorID,"
                     "  Time,"
                     "  Temperature,"
                     "  Humidity"
                     ") "
                     "VALUES ("
                     "  '" + sensorID.substr(0, 256) + "',"         // VARCHAR(256)
                     "  " + std::to_string(timestamp) + ","         // INT
                     "  " + std::to_string(data.temperature) + ","  // NUMERIC
                     "  " + std::to_string(data.humidity) + ","     // NUMERIC
                     ");";

        auto results = pimpl->exec(query, true);
    }

    //==============================================================================
    std::string DB::getName()
    {
        // If About table does not exist, create it, then store & return the default name
        if (!hasTable("About"))
        {
            createAboutTable();
            setName(PROJECT_NAME);

            return PROJECT_NAME;
        }

        // Find name in "About" table
        auto query = "SELECT Name FROM About";
        auto results = pimpl->exec(query, false);

        // Always use the first match (there should only ever be one row, but who knows!)
        if (results.size() > 0)
        {
            return results[0][0].as<std::string>();
        }
        else
        {
            g_logger.error("Expected 1 row in About table, got none!");
            return "";
        }
    }

    void DB::setName(std::string name)
    {
        // If About table does not exist, create it and save the given name
        if (!hasTable("About"))
        {
            createAboutTable();
        }

        // Update row in "About" table
        auto query = "UPDATE About SET Name = " + name;
        auto results = pimpl->exec(query, true);
    }

} // namespace beewatch
