//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/gpio.h"
#include "io/pigpio.h"

#include <algorithm>

namespace beewatch
{
    namespace io
    {

        //================================================================
        bool GPIO::_claimedGPIOList[NUM_GPIO] = { 0 };

        //================================================================
        GPIO::GPIO(const unsigned id)
        {
            // Ensure pigpio is initialised
            PiGPIOLib::init();

            // Take ownership of GPIO
            _id = id;
            _claimedGPIOList[id] = true;
        }

        GPIO::~GPIO()
        {
            // Reset GPIO configuration
            gpioSetMode(_id, PI_INPUT);
            gpioSetPullUpDown(_id, PI_PUD_OFF);

            // Relinquish GPIO ownership
            _claimedGPIOList[_id] = false;
        }


        //================================================================
        GPIO::Ptr GPIO::claim(unsigned gpioId)
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
        int GPIO::findFunction(Fn function)
        {
            return findFunction(_id, function);
        }

        int GPIO::findFunction(unsigned gpioId, Fn function)
        {
            auto gpioModes = gpioModeLookup[gpioId];

            for (int i = 0; i < NUM_MODES; ++i)
            {
                if (gpioModes[i] == function)
                {
                    return i;
                }
            }

            return -1;
        }


        //================================================================
        void GPIO::setDirection(Direction direction)
        {
            _direction = direction;

            // TODO: set GPIO direction
        }

        GPIO::Direction GPIO::getDirection()
        {
            return _direction;
        }


        //================================================================
        void GPIO::write(LogicalState state)
        {
            // TODO: write GPIO state
        }

        LogicalState GPIO::read() const
        {
            // TODO: read GPIO state

            return LogicalState::INVALID;
        }

        LogicalState GPIO::waitForStateChange()
        {
            // TODO: configure GPIO interrupt

            return LogicalState::INVALID;
        }

    } // namespace io
} // namespace beewatch
