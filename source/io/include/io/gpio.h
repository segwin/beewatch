//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <io/io_api.h>

#include "io.h"
#include "pigpio.h"

#include <array>
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
             * @enum GPIO functions
             */
            enum Fn
            {
                INVALID = -1,
                RESERVED = -2,

                // ARM JTAG
                ARM_TRST, ARM_RTCK, ARM_TDO, ARM_TCK, ARM_TDI, ARM_TMS,

                // Basic GPIO
                INPUT, OUTPUT,
                
                // BSC master (Broadcom Serial Control)
                SDA0, SCL0, SDA1, SCL1,

                // BSC slave
                BSCSL_SDA_MOSI, BSCSL_SCL_SCLK, BSCSL_MISO, BSCSL_CE_N,

                // General-purpose clocks
                GPCLK0, GPCLK1, GPCLK2,

                // PCM audio
                PCM_CLK, PCM_FS, PCM_DIN, PCM_DOUT,
                
                // PWM
                PWM0, PWM1,

                // Secondary memory interface
                SA0, SA1, SA2, SA3, SA4, SA5,
                SOE_N_SE, SWE_N_SRW_N,
                SD0, SD1, SD2, SD3, SD4, SD5, SD6, SD7, SD8, SD9,
                SD10, SD11, SD12, SD13, SD14, SD15, SD16, SD17,

                // SD
                SD1_CLK, SD1_CMD, SD1_DAT0, SD1_DAT1, SD1_DAT2, SD1_DAT3,

                // SPI
                SPI0_CE0_N, SPI0_CE1_N, SPI0_MISO, SPI0_MOSI, SPI0_SCLK,
                SPI1_CE0_N, SPI1_CE1_N, SPI1_CE2_N, SPI1_MISO, SPI1_MOSI, SPI1_SCLK,
                SPI2_CE0_N, SPI2_CE1_N, SPI2_CE2_N, SPI2_MISO, SPI2_MOSI, SPI2_SCLK,

                // UART
                TXD0, RXD0, CTS0, RTS0,
                TXD1, RXD1, CTS1, RTS1,
            };

            /**
             * Number of GPIO pins on the common Raspberry Pi boards (excluding
             * compute module)
             */
            static constexpr auto NUM_GPIO = 40;

            /**
             * Number of GPIO modes, including alternate functions
             */
            static constexpr auto NUM_MODES = 8;

            /**
             * Lookup table of modes & alternate functions for each GPIO. Pulled
             * from BCM2835 specification: "BCM2835 ARM Peripherals" (2012)
             */
            static const Fn gpioModeLookup[NUM_GPIO][NUM_MODES];

            /**
             * @brief Check if GPIO has given function
             *
             * @param [in] function Function to search for
             *
             * @returns Mode index if found, -1 otherwise
             */
            int findFunction(Fn function);

            /**
             * @brief Static method to check if given GPIO id allows a certain function
             *
             * @param [in] gpioId   ID of GPIO to search in
             * @param [in] function Function to search for
             *
             * @returns Mode index if found, -1 otherwise
             */
            static int findFunction(unsigned gpioId, Fn function);


            //================================================================
            /**
             * @enum GPIO direction
             */
            enum Direction
            {
                In,
                Out
            };

            /**
             * @brief Set GPIO direction
             *
             * @param [in] direction  Direction to use on GPIO
             */
            void setDirection(Direction direction);

            /**
             * @brief Get current GPIO direction
             *
             * @returns Current GPIO direction
             */
            Direction getDirection();


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
             * @brief Configures GPIO to signal all state changes
             */
            void enableEdgeDetection();

            /**
            * @brief Disables interrupt signaling on state changes
            */
            void disableEdgeDetection();

            /**
             * @brief Wait for GPIO state to change and return new value
             *
             * NB: This operation blocks the calling thread until the GPIO
             * changes state.
             *
             * @returns New GPIO state
             */
            LogicalState waitForStateChange();

            /**
             * @brief Wait for GPIO state to match given state
             *
             * Returns immediately if GPIO is already on given state.
             *
             * NB: This operation blocks the calling thread until the GPIO
             * changes state.
             */
            void waitForState(LogicalState state);

            /**
             * @brief Notify waiting threads about a state change
             *
             * Uses the ISR function format required by the pigpio library
             * (gpioISRFuncEx_t).
             *
             * @param unused
             * @param [in] level    New GPIO state (0 = LO, 1 = HI)
             * @param unused
             * @param [in] gpioPtr  Pointer to bound GPIO object
             */
            static void signalEdgeDetection(int, int level, uint32_t, void * gpioPtr);


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
            static bool _claimedGPIOList[NUM_GPIO];
            static std::mutex _claimMutex[NUM_GPIO];

            bool _edgeDetectionActive;
            std::mutex _stateChangeMutex;
            std::condition_variable _stateChangeVariable;
            LogicalState _currentState;


            //================================================================
            unsigned _id;

            Direction _direction;
        };

    } // namespace io
} // namespace beewatch
