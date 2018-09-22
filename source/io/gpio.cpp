//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/gpio.h"

#include "containers.h"

#include <wiringPi.h>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <vector>
#include <thread>

namespace beewatch
{
    namespace io
    {

        //================================================================
        const BidirectionalMap<LogicalState, int> mapLogicalStateToWiringPi = {
            { LogicalState::LO, LOW },
            { LogicalState::HI, HIGH },

            { LogicalState::Invalid, -1 }
        };

        const BidirectionalMap<GPIO::Mode, int> mapModeToWiringPi = {
            { GPIO::Mode::Input, INPUT },
            { GPIO::Mode::Output, OUTPUT },
            { GPIO::Mode::PWM, PWM_TONE_OUTPUT },
            { GPIO::Mode::CLK, GPIO_CLOCK },
        };

        const BidirectionalMap<GPIO::Resistor, int> mapResistorToWiringPi = {
            { GPIO::Resistor::Off, PUD_OFF },
            { GPIO::Resistor::PullUp, PUD_UP },
            { GPIO::Resistor::PullDown, PUD_DOWN },
        };

        const BidirectionalMap<GPIO::EdgeType, int> mapEdgeTypeToWiringPi = {
            { GPIO::EdgeType::None, INT_EDGE_SETUP },
            { GPIO::EdgeType::Rising, INT_EDGE_RISING },
            { GPIO::EdgeType::Falling, INT_EDGE_FALLING },
            { GPIO::EdgeType::Both, INT_EDGE_BOTH },
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

            if (wiringPiSetupPhys() < 0)
            {
                throw std::runtime_error("An error occurred while initialising gpio library, do we have root privileges?");
            }

            bIsInitialised = true;
        }


        //================================================================
        void GPIO::setMode(Mode mode)
        {
            pinMode(_id, mapModeToWiringPi.at1(mode));
            _mode = mode;
        }

        GPIO::Mode GPIO::getMode() const
        {
            return _mode;
        }


        //================================================================
        void GPIO::setResistorMode(Resistor cfg)
        {
            pullUpDnControl(_id, mapResistorToWiringPi.at1(cfg));
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

            digitalWrite(_id, mapLogicalStateToWiringPi.at1(state));
        }

        LogicalState GPIO::read()
        {
            assert(_mode != Mode::Output);

            return mapLogicalStateToWiringPi.at2(digitalRead(_id));
        }


        //================================================================
        void GPIO::setEdgeDetection(EdgeType type, void (*callback)(void))
        {
            if (type == EdgeType::None)
            {
                clearEdgeDetection();
            }

            wiringPiISR(_id, mapEdgeTypeToWiringPi.at1(type), callback);
            _edgeDetection = type;
        }

        GPIO::EdgeType GPIO::getEdgeDetection() const
        {
            return _edgeDetection;
        }

        static void nop() {}

        void GPIO::clearEdgeDetection()
        {
            wiringPiISR(_id, INT_EDGE_RISING, &nop);
            _edgeDetection = EdgeType::None;;
        }

    } // namespace io
} // namespace beewatch
