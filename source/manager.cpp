//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "manager.h"

#include "timing.h"


namespace beewatch
{
    
    //==============================================================================
    Manager::Manager()
    {
        // Initialise DHT11
        _climateSensor = std::make_unique<hw::DHTxx>(hw::DHTxx::Type::DHT22, io::GPIO::claim(16));

        // Initialise HX711
        //hx711 = std::make_unique<hw::HX711>(io::GPIO::claim(5), io::GPIO::claim(6));
    }

    //==============================================================================
    std::string Manager::getName() const
    {
        std::shared_lock<std::shared_mutex> readLock(_attrMutex);
        return _name;
    }

    void Manager::setName(std::string name)
    {
        std::unique_lock<std::shared_mutex> writeLock(_attrMutex);
        _name = name;
    }

    //==============================================================================
    std::map<time_t, ClimateData<double>> Manager::getClimateSamples(time_t since) const
    {
        std::shared_lock<std::shared_mutex> readLock(_attrMutex);

        // TODO: Implement storage class for sample data
        if (since > g_timeReal.now())
            return {};
        else
            return { { g_timeReal.now(), _climateSensor->read() } };
    }

    //==============================================================================
    void Manager::ctrlLoop()
    {
        while (1)
        {
            auto data = _climateSensor->read();

            g_logger.print(Logger::Info, "Humidity: " + numToStr(data.humidity) + " %");
            g_logger.print(Logger::Info, "Temperature: " + numToStr(data.temperature) + " deg Celsius");

            // Measure mass
            //auto mass = hx711->read();
            //
            //g_logger.print(Logger::Info, "Mass: " + numToStr(mass) + " kg");

            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

} // namespace beewatch
