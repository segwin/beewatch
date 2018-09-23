//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/gpio.h"

#include "containers.h"

#include <external/wiringPi.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <map>
#include <vector>
#include <thread>

namespace beewatch
{
    namespace io
    {

        //================================================================
        const BidirectionalMap<LogicalState, int> mapLogicalStateToWiringPi = {
            { LogicalState::LO, wiringPi.c_low },
            { LogicalState::HI, wiringPi.c_high },

            { LogicalState::Invalid, -1 }
        };

        const std::map<GPIO::Mode, int> mapModeToWiringPi = {
            { GPIO::Mode::Input, wiringPi.c_input },
            { GPIO::Mode::Output, wiringPi.c_output },
            { GPIO::Mode::PWM, wiringPi.c_pwmToneOutput },
            { GPIO::Mode::CLK, wiringPi.c_gpioClock },
        };

        const std::map<GPIO::Resistor, int> mapResistorToWiringPi = {
            { GPIO::Resistor::Off, wiringPi.c_pudOff },
            { GPIO::Resistor::PullUp, wiringPi.c_pudUp },
            { GPIO::Resistor::PullDown, wiringPi.c_pudDown },
        };

        const std::map<GPIO::EdgeType, int> mapEdgeTypeToWiringPi = {
            { GPIO::EdgeType::None, wiringPi.c_intEdgeSetup },
            { GPIO::EdgeType::Rising, wiringPi.c_intEdgeRising },
            { GPIO::EdgeType::Falling, wiringPi.c_intEdgeFalling },
            { GPIO::EdgeType::Both, wiringPi.c_intEdgeBoth },
        };

        //================================================================
        constexpr int GPIO::NUM_GPIO;

        bool GPIO::_claimedGPIOList[NUM_GPIO] = { 0 };
        std::mutex GPIO::_claimMutex[NUM_GPIO];

        //================================================================
        GPIO::GPIO(int id)
        {
            // Ensure GPIOs are initialised
            GPIO::Init();

            if (_claimedGPIOList[id])
            {
                throw std::invalid_argument("Requested GPIO is already claimed");
            }

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


        //================================================================
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

        GPIO::Ptr GPIO::claim(int gpioId)
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
            return GPIO::Ptr(new GPIO(gpioId));
        }


        //================================================================
        void GPIO::Init()
        {
            static bool bIsInitialised = false;

            if (bIsInitialised)
            {
                return;
            }

            bIsInitialised = true;
        }


        //================================================================
        void GPIO::setMode(Mode mode)
        {
            wiringPi.pinMode(_id, mapModeToWiringPi.at(mode));
            _mode = mode;
        }

        GPIO::Mode GPIO::getMode() const
        {
            return _mode;
        }


        //================================================================
        void GPIO::setResistorMode(Resistor cfg)
        {
            wiringPi.pullUpDnControl(_id, mapResistorToWiringPi.at(cfg));
            _resistorCfg = cfg;
        }

        GPIO::Resistor GPIO::getResistorMode() const
        {
            return _resistorCfg;
        }


        //================================================================
        void GPIO::write(LogicalState state)
        {
            assert(_mode != Mode::Input);
            assert(state != LogicalState::Invalid);

            wiringPi.digitalWrite(_id, mapLogicalStateToWiringPi.at1(state));
        }

        LogicalState GPIO::read()
        {
            assert(_mode != Mode::Output);

            return mapLogicalStateToWiringPi.at2(wiringPi.digitalRead(_id));
        }


        //================================================================
        void GPIO::setEdgeDetection(EdgeType type, void (*callback)(void))
        {
            if (type == EdgeType::None)
            {
                clearEdgeDetection();
            }

            wiringPi.setISR(_id, mapEdgeTypeToWiringPi.at(type), callback);
            _edgeDetection = type;
        }

        GPIO::EdgeType GPIO::getEdgeDetection() const
        {
            return _edgeDetection;
        }

        static void nop() {}

        void GPIO::clearEdgeDetection()
        {
            wiringPi.setISR(_id, wiringPi.c_intEdgeRising, &nop);
            _edgeDetection = EdgeType::None;;
        }

    } // namespace io
} // namespace beewatch
