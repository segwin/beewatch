//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/gpio.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <thread>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <wiringPi.h>

namespace beewatch
{
    namespace io
    {

        //================================================================
        bool GPIO::_claimedGPIOList[NUM_GPIO] = { 0 };
        std::mutex GPIO::_claimMutex[NUM_GPIO];

        //================================================================
        GPIO::GPIO(const unsigned id)
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
        GPIO::Ptr GPIO::claim(int gpioId)
        {
            std::lock_guard<std::mutex> lock(_claimMutex[gpioId]);

            // Check that requested GPIO is valid and available
            if (gpioId < 0 || gpioId > NUM_GPIO)
            {
                throw std::range_error("Requested GPIO is out of bounds");
            }
            else if (_claimedGPIOList[gpioId] == true)
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
            pinMode(_id, static_cast<int>(mode));
            _mode = mode;
        }

        GPIO::Mode GPIO::getMode() const
        {
            return _mode;
        }


        //================================================================
        void GPIO::setResistorMode(Resistor cfg)
        {
            pullUpDnControl(_id, static_cast<int>(cfg));
            _resistorCfg = cfg;
        }


        //================================================================
        void GPIO::write(LogicalState state)
        {
            assert(_mode != Mode::Input);
            assert(state != LogicalState::Invalid);

            digitalWrite(_id, static_cast<int>(state));
        }

        LogicalState GPIO::read()
        {
            assert(_mode != Mode::Output);

            return static_cast<LogicalState>(digitalRead(_id));
        }


        //================================================================
        void GPIO::setEdgeDetection(EdgeType type, void (*callback)(void))
        {
            if (type == EdgeType::None)
            {
                clearEdgeDetection();
            }

            wiringPiISR(_id, static_cast<int>(type), callback);
            _edgeDetection = type;
        }

        static void nop() {}

        void GPIO::clearEdgeDetection()
        {
            wiringPiISR(_id, INT_EDGE_RISING, &nop);
            _edgeDetection = EdgeType::None;;
        }

    } // namespace io
} // namespace beewatch
