//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/gpio.h"
#include "io/pigpio.h"

#include "pigpio/pigpio.h"

#include <algorithm>

namespace beewatch
{
    namespace io
    {

        //================================================================
        bool GPIO::_claimedGPIOList[NUM_GPIO] = { 0 };
        std::mutex GPIO::_claimMutex[NUM_GPIO];

        const GPIO::Fn GPIO::gpioModeLookup[NUM_GPIO][NUM_MODES] = {
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

        //================================================================
        GPIO::GPIO(const unsigned id)
        {
            // Ensure pigpio is initialised
            PiGPIOLib::init();

            // Take ownership of GPIO
            _id = id;
            _claimedGPIOList[id] = true;

            // Set initial direction to input
            setDirection(In);
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

        void GPIO::setMode(int mode)
        {
            gpioSetMode(_id, mode);
        }


        //================================================================
        void GPIO::setDirection(Direction direction)
        {
            unsigned mode;

            if (direction == In)
            {
                mode = PI_INPUT;
            }
            else
            {
                mode = PI_OUTPUT;
            }

            gpioSetMode(_id, mode);

            _direction = direction;
        }

        GPIO::Direction GPIO::getDirection()
        {
            return _direction;
        }


        //================================================================
        void GPIO::write(LogicalState state)
        {
            switch (state)
            {
            case LogicalState::HI:
                gpioWrite(_id, PI_ON);
                break;

            case LogicalState::LO:
                gpioWrite(_id, PI_OFF);
                break;

            default:

                break;
            }
        }

        LogicalState GPIO::read()
        {
            switch (gpioRead(_id))
            {
            case PI_ON:
                return LogicalState::HI;
                break;

            case PI_OFF:
                return LogicalState::LO;
                break;

            default:
                return LogicalState::INVALID;
                break;
            }
        }


        //================================================================
        void GPIO::enableEdgeDetection()
        {
            if (!_edgeDetectionActive)
            {
                gpioSetISRFuncEx(_id, EITHER_EDGE, -1, &GPIO::signalEdgeDetection, this);
                _edgeDetectionActive = true;
            }
        }

        void GPIO::disableEdgeDetection()
        {
            if (_edgeDetectionActive)
            {
                gpioSetISRFuncEx(_id, EITHER_EDGE, -1, nullptr, nullptr);
                _edgeDetectionActive = false;
            }
        }

        LogicalState GPIO::waitForStateChange()
        {
            // Ensure edge detection is activated
            enableEdgeDetection();

            // Wait for state change signal
            std::unique_lock<std::mutex> lock(_stateChangeMutex);
            _stateChangeVariable.wait(lock);

            return _currentState;
        }

        void GPIO::waitForState(LogicalState state)
        {
            if (read() == state)
            {
                return;
            }
            else
            {
                while (waitForStateChange() != state)
                {
                    // wait
                }
            }
        }

        void GPIO::signalEdgeDetection(int, int level, uint32_t, void * gpioPtr)
        {
            // De-obfuscate GPIO pointer
            GPIO * gpio = (GPIO*)gpioPtr;

            // Signal state change
            std::lock_guard<std::mutex> lock(gpio->_stateChangeMutex);
            gpio->_stateChangeVariable.notify_all();

            // Write post-transition GPIO state
            if (level > PI_OFF)
            {
                gpio->_currentState = LogicalState::HI;
            }
            else
            {
                gpio->_currentState = LogicalState::LO;
            }
        }

    } // namespace io
} // namespace beewatch
