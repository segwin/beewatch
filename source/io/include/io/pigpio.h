//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include "pigpio/pigpio.h"

#include <stdexcept>

namespace beewatch
{
    namespace io
    {

        //================================================================
        /**
        * @class PiGPIOScoped
        *
        * Basic wrapper implementing RAII semantics for the pigpio library
        */
        class PiGPIOLib
        {
        public:
            //================================================================
            /**
             * @brief Ensure pigpio library is initialised
             *
             * Creates singleton object for class to automate initialising
             * and terminating the pigpio library instance.
             * 
             * @returns pigpio version
             */
            static int init()
            {
                static PiGPIOLib pigpio;
                return _version;
            }

        private:
            //================================================================
            /**
             * @brief Calls pigpio's gpioInitialise() method and sets the version
             */
            PiGPIOLib()
            {
                _version = gpioInitialise();

                if (_version < 0)
                {
                    throw std::runtime_error("Failed to initialise pigpio library");
                }
            }
            
            /**
             * @brief Calls pigpio's gpioTerminate() method to clean up
             */
            ~PiGPIOLib()
            {
                gpioTerminate();
            }

            //================================================================
            static int _version;
        };

    } // namespace io
} // namespace beewatch