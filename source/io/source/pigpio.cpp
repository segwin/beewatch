//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/pigpio.h"

#include "pigpio/pigpio.h"

namespace beewatch
{
    namespace io
    {

        //================================================================
        int PiGPIOLib::_version = -1;
        

        //================================================================
        int PiGPIOLib::init()
        {
            static PiGPIOLib pigpio;
            return _version;
        }


        //================================================================
        PiGPIOLib::PiGPIOLib()
        {
            _version = gpioInitialise();

            if (_version < 0)
            {
                throw std::runtime_error("Failed to initialise pigpio library");
            }
        }

        PiGPIOLib::~PiGPIOLib()
        {
            gpioTerminate();
        }

    } // namespace io
} // namespace beewatch
