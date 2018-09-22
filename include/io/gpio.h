//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include "io.h"

#include <array>
#include <atomic>
#include <bitset>
#include <mutex>
#include <vector>


namespace beewatch
{
    namespace io
    {

        //================================================================
        /**
         * @enum LogicalState
         *
         * Models the logical state on a given IO port (HI/LO)
         */
        enum class LogicalState
        {
            LO,
            HI,

            Invalid
        };

        //================================================================
        /**
         * @class GPIO
         *
         * Models a GPIO interface
         */
        class GPIO : public InputOutput<LogicalState>
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
            static constexpr int NUM_GPIO = 40;

            /**
             * @brief Get list of unclaimed GPIOs
             *
             * @returns Vector containing IDs of all unclaimed GPIOs
             */
            static std::vector<int> getAvailableIDs();

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
            static GPIO::Ptr claim(int gpioId);


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
             * Lists the 4 supported GPIO modes
             */
            enum class Mode : int
            {
                Input,
                Output,
                PWM,
                CLK,
            };

            /**
             * @brief Set GPIO mode
             *
             * @param [in] mode Mode to use on GPIO
             */
            void setMode(Mode mode);

            /**
             * @brief Get GPIO mode
             *
             * @returns Current GPIO mode
             */
            Mode getMode() const;


            //================================================================
            /**
             * @enum Resistor
             *
             * Lists the different GPIO resistor configurations (pull-up, pull-
             * down and no resistor)
             */
            enum class Resistor : int
            {
                Off,
                PullUp,
                PullDown,
            };

            /**
             * @brief Set resistor configuration (pull-up, pull-down or none)
             *
             * @param [in] cfg  Resistor configuration
             */
            void setResistorMode(Resistor mode);

            /**
             * @brief Get resistor configuration
             *
             * @returns Current resistor configuration
             */
            Resistor getResistorMode() const;


            //================================================================
            enum class EdgeType : int
            {
                None,
                Rising,
                Falling,
                Both,
            };

            /**
             * @brief Configures GPIO to detect states
             *
             * @param [in] type     Type of state change to detect
             * @param [in] callback Callback function to execute in ISR
             */
            void setEdgeDetection(EdgeType type, void (*callback)(void));

            /**
             * @brief Get GPIO edge detection mode
             *
             * @returns Current GPIO edge detection mode
             */
            EdgeType getEdgeDetection() const;


            /**
            * @brief Disables state detection
            */
            void clearEdgeDetection();


            //================================================================
            /**
             * @brief Get ID of GPIO modeled by object
             *
             * @returns GPIO number
             */
            int getID() { return _id; }


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
            GPIO(int id);


        private:
            //================================================================
            /**
             * @brief Initialise GPIO memory access
             *
             * Maps GPIO peripherals to memory. Does nothing when called after
             * first initialisation.
             */
            static void Init();
            

            //================================================================
            static bool _claimedGPIOList[NUM_GPIO];
            static std::mutex _claimMutex[NUM_GPIO];


            //================================================================
            int _id;

            Mode _mode;
            Resistor _resistorCfg;
            EdgeType _edgeDetection;
        };

    } // namespace io
} // namespace beewatch
