//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <io/io_api.h>

#include <stdexcept>

namespace beewatch
{
    namespace io
    {

        //================================================================
        /**
        * @class PiGPIOLib
        *
        * Basic wrapper implementing RAII semantics for the pigpio library
        */
        class IO_API PiGPIOLib
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
            static IO_API int init();

        private:
            //================================================================
            /**
             * @brief Calls pigpio's gpioInitialise() method and sets the version
             */
            PiGPIOLib();
            
            /**
             * @brief Calls pigpio's gpioTerminate() method to clean up
             */
            ~PiGPIOLib();

            //================================================================
            static int _version;
        };

    } // namespace io
} // namespace beewatch
