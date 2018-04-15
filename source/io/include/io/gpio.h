//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include "io.h"

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
            static constexpr Fn gpioModeLookup[NUM_GPIO][NUM_MODES] = {
                {INPUT, OUTPUT, SDA0,       SA5,         RESERVED, INVALID,        INVALID,    INVALID},  // GPIO0
                {INPUT, OUTPUT, SCL0,       SA4,         RESERVED, INVALID,        INVALID,    INVALID},  // GPIO1
                {INPUT, OUTPUT, SDA1,       SA3,         RESERVED, INVALID,        INVALID,    INVALID},  // GPIO2
                {INPUT, OUTPUT, SCL1,       SA2,         RESERVED, INVALID,        INVALID,    INVALID},  // GPIO3
                {INPUT, OUTPUT, GPCLK0,     SA1,         RESERVED, INVALID,        INVALID,    ARM_TDI},  // GPIO4
                {INPUT, OUTPUT, GPCLK1,     SA0,         RESERVED, INVALID,        INVALID,    ARM_TDO},  // GPIO5
                {INPUT, OUTPUT, GPCLK2,     SOE_N_SE,    RESERVED, INVALID,        INVALID,    ARM_RTCK}, // GPIO6
                {INPUT, OUTPUT, SPI0_CE1_N, SWE_N_SRW_N, RESERVED, INVALID,        INVALID,    INVALID},  // GPIO7
                {INPUT, OUTPUT, SPI0_CE0_N, SD0,         RESERVED, INVALID,        INVALID,    INVALID},  // GPIO8
                {INPUT, OUTPUT, SPI0_MISO,  SD1,         RESERVED, INVALID,        INVALID,    INVALID},  // GPIO9
                {INPUT, OUTPUT, SPI0_MOSI,  SD2,         RESERVED, INVALID,        INVALID,    INVALID},  // GPIO10
                {INPUT, OUTPUT, SPI0_SCLK,  SD3,         RESERVED, INVALID,        INVALID,    INVALID},  // GPIO11
                {INPUT, OUTPUT, PWM0,       SD4,         RESERVED, INVALID,        INVALID,    ARM_TMS},  // GPIO12
                {INPUT, OUTPUT, PWM1,       SD5,         RESERVED, INVALID,        INVALID,    ARM_TCK},  // GPIO13
                {INPUT, OUTPUT, TXD0,       SD6,         RESERVED, INVALID,        INVALID,    TXD1},     // GPIO14
                {INPUT, OUTPUT, RXD0,       SD7,         RESERVED, INVALID,        INVALID,    RXD1},     // GPIO15
                {INPUT, OUTPUT, RESERVED,   SD8,         RESERVED, CTS0,           SPI1_CE2_N, CTS1},     // GPIO16
                {INPUT, OUTPUT, RESERVED,   SD9,         RESERVED, RTS0,           SPI1_CE1_N, RTS1},     // GPIO17
                {INPUT, OUTPUT, PCM_CLK,    SD10,        RESERVED, BSCSL_SDA_MOSI, SPI1_CE0_N, PWM0},     // GPIO18
                {INPUT, OUTPUT, PCM_FS,     SD11,        RESERVED, BSCSL_SCL_SCLK, SPI1_MISO,  PWM1},     // GPIO19
                {INPUT, OUTPUT, PCM_DIN,    SD12,        RESERVED, BSCSL_MISO,     SPI1_MOSI,  GPCLK0},   // GPIO20
                {INPUT, OUTPUT, PCM_DOUT,   SD13,        RESERVED, BSCSL_CE_N,     SPI1_SCLK,  GPCLK1},   // GPIO21
                {INPUT, OUTPUT, RESERVED,   SD14,        RESERVED, SD1_CLK,        ARM_TRST,   INVALID},  // GPIO22
                {INPUT, OUTPUT, RESERVED,   SD15,        RESERVED, SD1_CMD,        ARM_RTCK,   INVALID},  // GPIO23
                {INPUT, OUTPUT, RESERVED,   SD16,        RESERVED, SD1_DAT0,       ARM_TDO,    INVALID},  // GPIO24
                {INPUT, OUTPUT, RESERVED,   SD17,        RESERVED, SD1_DAT1,       ARM_TCK,    INVALID},  // GPIO25
                {INPUT, OUTPUT, RESERVED,   RESERVED,    RESERVED, SD1_DAT2,       ARM_TDI,    INVALID},  // GPIO26
                {INPUT, OUTPUT, RESERVED,   RESERVED,    RESERVED, SD1_DAT3,       ARM_TMS,    INVALID},  // GPIO27
                {INPUT, OUTPUT, SDA0,       SA5,         PCM_CLK,  RESERVED,       INVALID,    INVALID},  // GPIO28
                {INPUT, OUTPUT, SCL0,       SA4,         PCM_FS,   RESERVED,       INVALID,    INVALID},  // GPIO29
                {INPUT, OUTPUT, RESERVED,   SA3,         PCM_DIN,  CTS0,           INVALID,    CTS1},     // GPIO30
                {INPUT, OUTPUT, RESERVED,   SA2,         PCM_DOUT, RTS0,           INVALID,    RTS1},     // GPIO31
                {INPUT, OUTPUT, GPCLK0,     SA1,         RESERVED, TXD0,           INVALID,    TXD1},     // GPIO32
                {INPUT, OUTPUT, RESERVED,   SA0,         RESERVED, RXD0,           INVALID,    RXD1},     // GPIO33
                {INPUT, OUTPUT, GPCLK0,     SOE_N_SE,    RESERVED, RESERVED,       INVALID,    INVALID},  // GPIO34
                {INPUT, OUTPUT, SPI0_CE1_N, SWE_N_SRW_N, INVALID,  RESERVED,       INVALID,    INVALID},  // GPIO35
                {INPUT, OUTPUT, SPI0_CE0_N, SD0,         TXD0,     RESERVED,       INVALID,    INVALID},  // GPIO36
                {INPUT, OUTPUT, SPI0_MISO,  SD1,         RXD0,     RESERVED,       INVALID,    INVALID},  // GPIO37
                {INPUT, OUTPUT, SPI0_MOSI,  SD2,         RTS0,     RESERVED,       INVALID,    INVALID},  // GPIO38
                {INPUT, OUTPUT, SPI0_SCLK,  SD3,         CTS0,     RESERVED,       INVALID,    INVALID}   // GPIO39
            };

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
                IN,
                OUT
            };

            /**
             * @brief Set GPIO direction
             *
             * @param [in] direction  Direction to use on GPIO
             */
            virtual void setDirection(Direction direction);

            /**
             * @brief Get current GPIO direction
             *
             * @returns Current GPIO direction
             */
            virtual Direction getDirection();

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
            virtual LogicalState read() const override;

            /**
             * @brief Wait for GPIO state to change and return new value
             *
             * NB: This operation blocks the calling thread until the GPIO
             * changes state.
             *
             * @returns New GPIO state
             */
            virtual LogicalState waitForStateChange();

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

            /**
             * @brief Relinquish ownership of and destroy GPIO object
             */
            virtual ~GPIO();

        private:
            //================================================================
            static bool _claimedGPIOList[NUM_GPIO];
            static std::mutex _claimMutex[NUM_GPIO];

            //================================================================
            unsigned _id;

            Direction _direction;
        };

    } // namespace io
} // namespace beewatch