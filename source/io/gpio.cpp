//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "io/gpio.h"

#include "util/logging.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <vector>
#include <thread>

namespace beewatch::io
{

    //==============================================================================
    constexpr int GPIO::NUM_GPIO;

    bool GPIO::_claimedGPIOList[NUM_GPIO] = { 0 };
    std::mutex GPIO::_claimMutex[NUM_GPIO];

    //==============================================================================
    GPIO::GPIO(int id, std::shared_ptr<external::IWiringPi> wiringPi)
        : _wiringPi(wiringPi)
    {
        if (_claimedGPIOList[id])
        {
            throw std::invalid_argument("Requested GPIO is already claimed");
        }

        // Set enum _maps
        _mapLogicalStateToWiringPi = {
            { LogicalState::LO, _wiringPi->c_low },
            { LogicalState::HI, _wiringPi->c_high },

            { LogicalState::Invalid, -1 }
        };

        _mapModeToWiringPi = {
            { GPIO::Mode::Input, _wiringPi->c_input },
            { GPIO::Mode::Output, _wiringPi->c_output },
            { GPIO::Mode::PWM, _wiringPi->c_pwmToneOutput },
            { GPIO::Mode::CLK, _wiringPi->c_gpioClock },
        };

        _mapResistorToWiringPi = {
            { GPIO::Resistor::Off, _wiringPi->c_pudOff },
            { GPIO::Resistor::PullUp, _wiringPi->c_pudUp },
            { GPIO::Resistor::PullDown, _wiringPi->c_pudDown },
        };

        _mapEdgeTypeToWiringPi = {
            { GPIO::EdgeType::None, _wiringPi->c_intEdgeSetup },
            { GPIO::EdgeType::Rising, _wiringPi->c_intEdgeRising },
            { GPIO::EdgeType::Falling, _wiringPi->c_intEdgeFalling },
            { GPIO::EdgeType::Both, _wiringPi->c_intEdgeBoth },
        };

        // Take ownership of GPIO
        _id = id;
        _claimedGPIOList[id] = true;

        // Set mode to input
        setMode(Mode::Input);
    }

    GPIO::~GPIO()
    {
        // Reset GPIO configuration
        setMode(Mode::Input);
        setResistorMode(Resistor::Off);
        clearEdgeDetection();

        // Relinquish GPIO ownership
        _claimedGPIOList[_id] = false;
    }


    //==============================================================================
    std::vector<int> GPIO::getAvailableIDs()
    {
        std::vector<int> availableIDs;
        
        for (int i = 0; i < NUM_GPIO; ++i)
        {
            std::lock_guard<std::mutex> guard(_claimMutex[i]);

            if (!_claimedGPIOList[i])
            {
                availableIDs.push_back(i);
            }
        }

        return availableIDs;
    }

    GPIO::Ptr GPIO::claim(int gpioId, std::shared_ptr<external::IWiringPi> wiringPi)
    {
        // Check that requested GPIO is valid and available
        if (gpioId < 0 || gpioId >= NUM_GPIO)
        {
            throw std::range_error("Requested GPIO is out of bounds");
        }

        std::lock_guard<std::mutex> guard(_claimMutex[gpioId]);

        if (_claimedGPIOList[gpioId] == true)
        {
            return nullptr;
        }

        // Construct GPIO object with ownership of given ID
        return GPIO::Ptr(new GPIO(gpioId, wiringPi));
    }


    //==============================================================================
    void GPIO::setMode(Mode mode)
    {
        _wiringPi->pinMode(_id, _mapModeToWiringPi.at(mode));
        _mode = mode;
    }

    GPIO::Mode GPIO::getMode() const
    {
        return _mode;
    }


    //==============================================================================
    void GPIO::setResistorMode(Resistor cfg)
    {
        _wiringPi->pullUpDnControl(_id, _mapResistorToWiringPi.at(cfg));
        _resistorCfg = cfg;
    }

    GPIO::Resistor GPIO::getResistorMode() const
    {
        return _resistorCfg;
    }


    //==============================================================================
    void GPIO::write(LogicalState state)
    {
        if (_mode == Mode::Input)
        {
            throw std::invalid_argument("write() called on GPIO configured as input");
        }

        if (state == LogicalState::Invalid)
        {
            throw std::invalid_argument("Received invalid logical state in GPIO::write()");
        }

        _wiringPi->digitalWrite(_id, _mapLogicalStateToWiringPi.at(state));
    }

    LogicalState GPIO::read()
    {
        if (_mode != Mode::Input)
        {
            throw std::invalid_argument("read() called on GPIO not configured as input");
        }

        int val = _wiringPi->digitalRead(_id);

        if (val == _wiringPi->c_low)
        {
            return LogicalState::LO;
        }
        else if (val == _wiringPi->c_high)
        {
            return LogicalState::HI;
        }
        else
        {
            g_logger.print(Logger::Error, "GPIO read returned an invalid value: " + std::to_string(val));
            return LogicalState::Invalid;
        }
    }


    //==============================================================================
    void GPIO::setEdgeDetection(EdgeType type, void (*callback)(void))
    {
        if (type == EdgeType::None)
        {
            clearEdgeDetection();
            return;
        }

        _wiringPi->setISR(_id, _mapEdgeTypeToWiringPi.at(type), callback);
        _edgeDetection = type;
    }

    GPIO::EdgeType GPIO::getEdgeDetection() const
    {
        return _edgeDetection;
    }

    void GPIO::clearEdgeDetection()
    {
        _wiringPi->setISR(_id, _wiringPi->c_intEdgeRising, &nop);
        _edgeDetection = EdgeType::None;
    }

} // namespace beewatch::io
