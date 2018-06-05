//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <io/io_api.h>

#include "io.h"

#include <array>
#include <bitset>
#include <mutex>

namespace beewatch
{
    namespace io
    {


        //================================================================
        /**
         * @class GPIO
         *
         * Models a GPIO interface
         */
        class IO_API GPIO : public InputOutput<LogicalState>
        {
        public:
            //================================================================
            /**
             * Unique pointer to a GPIO object
             *
             * NB: We use a unique pointer for this instead of the standard
             *     shared pointer since we want each GPIO to have unique ownership.
             *     This enforces move semantics whenever the GPIO is passed around,
             *     forcing callers to relinquish the GPIO before it can be used
             *     elsewhere.
             */
            using Ptr = std::unique_ptr<GPIO>;


            //================================================================
            /**
             * @brief Relinquish ownership of and destroy GPIO object
             */
            virtual ~GPIO();


            //================================================================
            /**
             * Number of GPIO pins on the common RPi boards (excluding compute)
             */
            static constexpr size_t NUM_GPIO = 40;

            /**
             * @brief Attempts to create a GPIO object for the given ID
             *
             * Checks if requested GPIO is unclaimed. If so, a GPIO object is
             * constructed and the ownership flag for that ID is set. Otherwise,
             * no object is constructed and the method returns nullptr.
             *
             * @param [in] gpioId   Desired GPIO ID
             *
             * @returns Shared pointer to GPIO object if successful, nullptr otherwise
             */
            static GPIO::Ptr claim(unsigned gpioId);


            //================================================================
            /**
             * @brief Write state (HI/LO) to GPIO
             *
             * This operation is "sticky", i.e. the value written remains the
             * state of the GPIO until a new value is given.
             *
             * @param [in] state    State to write to GPIO
             */
            virtual void write(LogicalState state) override;

            /**
             * @brief Get current state (HI/LO) on GPIO
             *
             * @returns Current GPIO state
             */
            virtual LogicalState read() override;


            //================================================================
            /**
             * @enum
             *
             * Lists the 8 possible GPIO modes (input, output and up to 6
             * alternate functions)
             */
            enum class Fn : uint32_t
            {
                Input  = 0x0,
                Output = 0x1,
                AltFn0 = 0x4,
                AltFn1 = 0x5,
                AltFn2 = 0x6,
                AltFn3 = 0x7,
                AltFn4 = 0x3,
                AltFn5 = 0x2,
            };

            /**
             * @brief Set GPIO function
             *
             * @param [in] function Function to use on GPIO
             */
            void setFunction(Fn function);

            /**
             * @brief Get GPIO mode
             *
             * @returns Current GPIO mode
             */
            Fn getFunction();


            //================================================================
            /**
             * @enum Resistor
             *
             * Lists the different GPIO resistor configurations (pull-up, pull-
             * down and no resistor)
             */
            enum class Resistor : uint32_t
            {
                Off      = 0x0,
                PullUp   = 0x1,
                PullDown = 0x2,
            };

            /**
             * @brief Set resistor configuration (pull-up, pull-down or none)
             *
             * @param [in] cfg  Resistor configuration
             */
            void setResistorMode(Resistor mode);


            //================================================================
            struct Edge
            {
                enum Type
                {
                    None         = 0,
                    Rising       = 1 << 0,
                    Falling      = 1 << 1,
                    High         = 1 << 2,
                    Low          = 1 << 3,
                    RisingAsync  = 1 << 4,
                    FallingAsync = 1 << 5,
                };
            };

            /**
             * @brief Configures GPIO to detect states
             *
             * @param [in] edgeTypes    Types of states to detect
             */
            void setEdgeDetection(Edge::Type edgeTypes);

            /**
            * @brief Disables state detection
            */
            void clearEdgeDetection();

            /**
             * @brief Wait for GPIO state to change and return new value
             *
             * NB: This operation blocks the calling thread until the GPIO
             * changes state.
             *
             * @returns New GPIO state
             */
            LogicalState waitForStateChange();


            //================================================================
            /**
             * @brief Get ID of GPIO modeled by object
             *
             * @returns GPIO number
             */
            unsigned getId() { return _id; }


        protected:
            //================================================================
            /**
             * @brief Construct and claim GPIO#, where # = id
             *
             * Constructor is protected and cannot be used outside of class.
             * Other classes should use the claim() factory method to try
             * and obtain a GPIO.
             *
             * @param [in] id   ID of GPIO pin to model
             */
            GPIO(const unsigned id);


        private:
            //================================================================
            /**
             * @brief Initialise GPIO memory access
             *
             * Maps GPIO peripherals to memory. Does nothing when called after
             * first initialisation.
             */
            static void Init();

            /**
             * @brief Set a register bit
             *
             * @param [in] 
             */
            template <int nbBits>
            inline void SetRegister(uint32_t val, const uint32_t offsets[]);

            static void * _gpioMmap;
            static volatile uint32_t * _gpioAddr;

            //================================================================
            static bool _claimedGPIOList[NUM_GPIO];
            static std::mutex _claimMutex[NUM_GPIO];

            Edge::Type _activeEdgeDetection;
            LogicalState _currentState;


            //================================================================
            unsigned _id;

            Fn _function;
            Resistor _resistorCfg;
        };

    } // namespace io
} // namespace beewatch
