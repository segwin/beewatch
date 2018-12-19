//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "ctrl/climate_ctrl.h"

#include "util/algorithms.hpp"
#include "global/logging.h"

#include <cassert>

namespace beewatch::ctrl
{

    using hw::DHTxx;
    using hw::Fan;
    
    //==============================================================================
    constexpr Range<double> ClimateCtrl::temperatureRange;
    constexpr double ClimateCtrl::temperatureTarget;
    constexpr Range<double> ClimateCtrl::humidityRange;
    constexpr double ClimateCtrl::humidityTarget;
    
    //==============================================================================
    ClimateCtrl::ClimateCtrl(std::vector<DHTxx::Ptr>&& sensors, Fan::Ptr fan)
    {
        if (contains(sensors.begin(), sensors.end(), nullptr))
        {
            throw std::invalid_argument("Received undefined sensor");
        }

        _sensors = std::move(sensors);

        if (!fan)
        {
            throw std::invalid_argument("Received undefined fan");
        }

        // Take ownership of fan
        _fan = std::move(fan);

        // Start system
        start();
    }

    ClimateCtrl::~ClimateCtrl()
    {
        if (_isActive)
        {
            stop();
        }
    }

    //==============================================================================
    void ClimateCtrl::start()
    {
        std::lock_guard<std::mutex> lock(_ctrlMutex);

        assert(!_isActive);

        _stopRequested = false;
        _ctrlThread = std::make_unique<std::thread>(&ClimateCtrl::ctrlLoop, this);
    }

    void ClimateCtrl::stop()
    {
        std::lock_guard<std::mutex> lock(_ctrlMutex);

        assert(_isActive);

        _stopRequested = true;
        _ctrlThread->join();
    }

    //==============================================================================
    void ClimateCtrl::getClimateData()
    {
        DHTxx::Data avgData;

        for (auto& sensor : _sensors)
        {
            // TODO: Handle read errors
            avgData += sensor->read();
        }

        avgData /= _sensors.size();

        {
            std::lock_guard<std::mutex> guard(_climateDataMutex);
            _climateData = avgData;
        }

        // Validate read results
        if ( !temperatureRange.contains(_climateData.temperature) )
        {
            g_logger.print(Logger::Warning, "Temperature is out of bounds: " + std::to_string(_climateData.temperature) + "deg Celsius");
        }

        if ( !humidityRange.contains(_climateData.humidity) )
        {
            g_logger.print(Logger::Warning, "Humidity is out of bounds: got " + std::to_string(_climateData.humidity) + "%");
        }
    }

    void ClimateCtrl::setFanSpeed()
    {
        /**
         * Fan speed is directly proportional to both temperature & humidity
         *
         * Priority:
         *     1) Temperature
         *     2) Humidity
         */
        double fanSpeedRpm;
        std::lock_guard<std::mutex> guard(_climateDataMutex);

        // TODO: Add hysteresis to control
        
        // Handle values above thresholds before anything else
        if (_climateData.temperature > temperatureRange.max || _climateData.humidity > humidityRange.max)
        {
            fanSpeedRpm = _fan->getMaxSpeedRpm();
        }
        // Handle high temperature
        else if (_climateData.temperature > temperatureRange.mid())
        {
            double relativeDistance = (_climateData.temperature - temperatureRange.mid()) / temperatureRange.max;
            fanSpeedRpm = _fan->getMaxSpeedRpm() / relativeDistance;
        }
        // Handle high humidity if temperature is not too low
        else if (_climateData.temperature > temperatureRange.min && _climateData.humidity > humidityRange.mid())
        {
            double relativeDistance = (_climateData.humidity - humidityRange.mid()) / humidityRange.max;
            fanSpeedRpm = _fan->getMaxSpeedRpm() / relativeDistance;
        }
        else
        {
            fanSpeedRpm = 0.0;
        }

        _fan->write(fanSpeedRpm);
    }

    //==============================================================================
    void ClimateCtrl::ctrlLoop()
    {
        std::unique_lock<std::mutex> lock(_ctrlMutex);

        while (!_stopRequested)
        {
            lock.unlock();

            getClimateData();
            setFanSpeed();
            
            // XXX: How often should we update the control state?
            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            lock.lock();
        }
    }

} // namespace beewatch::ctrl
