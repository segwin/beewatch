//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <cstdint>

namespace beewatch
{
    namespace bcm2835_periph 
    {

        using addr_t = uint32_t const;
        using offset_t = uint32_t const;

        addr_t BASE = 0x20000000;

        /**
         * GPIO
         */
        offset_t GPIO_OFFSET = 0x200000;
        addr_t GPIO_BASE = BASE + GPIO_OFFSET;

        /// FSEL
        offset_t GPIO_FSEL[] =  // GPIO function select
        {
            0x00,   // GPFSEL0
            0x04,   // GPFSEL1
            0x08,   // GPFSEL2
            0x0C,   // GPFSEL3
            0x10,   // GPFSEL4
            0x14,   // GPFSEL5
        };

        /// SET
        offset_t GPIO_SET[] =   // GPIO pin output set
        {
            0x1C,   // GPSET0
            0x20,   // GPSET1
        };

        /// CLR
        offset_t GPIO_CLR[] =   // GPIO pin output clear
        {
            0x28,   // GPCLR0
            0x2C,   // GPCLR1
        };

        /// LEV
        offset_t GPIO_LEV[] =   // GPIO pin level
        {
            0x34,   // GPLEV0
            0x38,   // GPLEV1
        };

        /// EDS
        offset_t GPIO_EDS[] =   // GPIO pin event detect status
        {
            0x40,   // GPEDS0
            0x44,   // GPEDS1
        };

        /// REN
        offset_t GPIO_REN[] =   // GPIO pin rising edge detect enable
        {
            0x4C,   // GPREN0
            0x50,   // GPREN1
        };

        /// FEN
        offset_t GPIO_FEN[] =   // GPIO pin falling edge detect enable
        {
            0x58,   // GPFEN0
            0x5C,   // GPFEN1
        };

        /// HEN
        offset_t GPIO_HEN[] =   // GPIO pin high detect enable
        {
            0x64,   // GPHEN0
            0x68,   // GPHEN1
        };

        /// LEN
        offset_t GPIO_LEN[] =   // GPIO pin low detect enable
        {
            0x70,   // GPLEN0
            0x74,   // GPLEN1
        };

        /// AREN
        offset_t GPIO_AREN[] =  // GPIO pin async. rising edge detect enable
        {
            0x7C,   // GPAREN0
            0x80,   // GPAREN1
        };

        /// AFEN
        offset_t GPIO_AFEN[] =  // GPIO pin async. falling edge detect enable
        {
            0x88,   // GPAFEN0
            0x8C,   // GPAFEN1
        };

        /// PUD
        offset_t GPIO_PUD =     // GPIO pin pull up/down enable
            0x94;   // GPPUD

        offset_t GPIO_PUDCLK[] = // GPIO pin pull up/down clock enable
        {
            0x98,   // GPPUDCLK0
            0x9C,   // GPPUDCLK1
        };

    } // namespace bcm2835_periph
} //namespace beewatch

