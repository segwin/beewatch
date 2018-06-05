//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/gpio.h"
#include "io/bcm2835.h"

#include <util/assert.h>

#include <algorithm>
#include <cstring>
#include <thread>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace beewatch
{
    namespace io
    {

        //================================================================
        bool GPIO::_claimedGPIOList[NUM_GPIO] = { 0 };
        std::mutex GPIO::_claimMutex[NUM_GPIO];

        //================================================================
        GPIO::GPIO(const unsigned id)
        {
            // Ensure GPIOs are initialised
            GPIO::Init();

            if (_claimedGPIOList[id])
            {
                throw std::invalid_argument("Requested GPIO is already claimed");
            }

            // Take ownership of GPIO
            _id = id;
            _claimedGPIOList[id] = true;

            // Set mode to input
            setFunction(Fn::Input);
        }

        GPIO::~GPIO()
        {
            // Reset GPIO configuration
            setFunction(Fn::Input);
            setResistorMode(Resistor::Off);
            disableEdgeDetection();

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
        void * GPIO::_gpioMmap = nullptr;
        volatile uint32_t * _gpio = nullptr;

        void GPIO::Init()
        {
            // Based on sample code from:
            // https://elinux.org/RPi_GPIO_Code_Samples#Direct_register_access
            
            if (_gpio != nullptr)
            {
                return;
            }
            
            // Open /dev/mem
            int memFd = open("/dev/mem", O_RDWR | O_SYNC);

            if (memFd < 0)
            {
                std::string error = strerror(errno);
                throw std::runtime_error("Failed to open /dev/mem: \"" + error + "\"");
            }

            // Create mmap for GPIO peripherals
            _gpioMmap = mmap( nullptr,                     // Any address will do
                              4*1024,                      // Map size
                              PROT_READ | PROT_WRITE,      // Enable RW access to mapped memory
                              MAP_SHARED,                  // Shared with other processes
                              memFd,                       // File descriptor to map (/dev/mem)
                              bcm2835_periph::GPIO_BASE ); // Offset to GPIO peripheral

            close(memFd);

            if (_gpioMmap == MAP_FAILED)
            {
                std::string error = strerror(errno);
                throw std::runtime_error("Failed to create mmap for GPIO peripherals: \"" + error + "\"");
            }

            _gpio = (volatile uint32_t*)_gpioMmap;
        }

        template <int nbBits>
        inline void GPIO::SetRegister(uint32_t val, const uint32_t offsets[])
        {
            static const int nbRegistersPerBank = 32 / nbBits;

            // Calculate address & bitshift
            uint32_t offset = offsets[_id / nbRegistersPerBank];
            volatile uint32_t * addr = _gpioAddr + offset;

            uint32_t shift = (_id % nbRegistersPerBank) * nbBits;

            // Clear and/or set register value
            if (!val || nbBits > 1)
            {
                static const uint32_t clearVal = (0x1 << nbBits) - 1;

                *addr &= ~(clearVal << shift);
            }

            if (val)
            {
                *addr |= (val << shift);
            }
        }


        //================================================================
        void GPIO::setFunction(Fn function)
        {
            // Set new function
            SetRegister<3>((uint32_t)function, bcm2835_periph::GPIO_FSEL);
        }

        GPIO::Fn GPIO::getFunction()
        {
            return _function;
        }


        //================================================================
        void GPIO::setResistorMode(Resistor mode)
        {
            // Calculate addresses and bitshifts
            uint32_t pudOffset = bcm2835_periph::GPIO_PUD;
            volatile uint32_t * pud = _gpioAddr + pudOffset;

            // 1. Write to GPPUD
            *pud = (uint32_t)mode;

            // 2. Wait ~150 cycles @ 700-1.2 GHz
            std::this_thread::sleep_for(std::chrono::nanoseconds(200));

            // 3. Write to GPPUDCLK
            SetRegister<1>(1, bcm2835_periph::GPIO_PUDCLK);

            // 4. Wait another ~150 cycles
            std::this_thread::sleep_for(std::chrono::nanoseconds(200));

            // 5. Clear GPPUD/GPPUDCLK signals
            SetRegister<1>(0, bcm2835_periph::GPIO_PUDCLK);
            *pud = 0;
        }


        //================================================================
        void GPIO::write(LogicalState state)
        {
            dbgAssert(_function != Fn::Input);

            if (state == LogicalState::Invalid)
            {
                throw std::invalid_argument("Received invalid write value");
            }

            // Write to SET (HI) and CLR (LO) registers
            if (state == LogicalState::HI)
            {
                SetRegister<1>(0, bcm2835_periph::GPIO_CLR);
                SetRegister<1>(1, bcm2835_periph::GPIO_SET);
            }
            else
            {
                SetRegister<1>(0, bcm2835_periph::GPIO_SET);
                SetRegister<1>(1, bcm2835_periph::GPIO_CLR);
            }
        }

        LogicalState GPIO::read()
        {
            dbgAssert(_function != Fn::Output);

            // Calculate address & bitshift
            uint32_t offset = bcm2835_periph::GPIO_LEV[_id / 32];
            volatile uint32_t * lev = _gpioAddr + offset;

            uint32_t shift = _id % 32;

            // Read register
            if ( *lev & (0x1 << shift) )
            {
                return LogicalState::HI;
            }
            else
            {
                return LogicalState::LO;
            }
        }


        //================================================================
        void GPIO::setEdgeDetection(Edge::Type edgeTypes)
        {
            if (_activeEdgeDetection == edgeTypes)
            {
                return;
            }

            // Reset detection registers based on given type mask
            SetRegister<1>((edgeTypes & Edge::Rising)       >> 0, bcm2835_periph::GPIO_REN);
            SetRegister<1>((edgeTypes & Edge::Falling)      >> 1, bcm2835_periph::GPIO_FEN);
            SetRegister<1>((edgeTypes & Edge::High)         >> 2, bcm2835_periph::GPIO_HEN);
            SetRegister<1>((edgeTypes & Edge::Low)          >> 3, bcm2835_periph::GPIO_LEN);
            SetRegister<1>((edgeTypes & Edge::RisingAsync)  >> 4, bcm2835_periph::GPIO_AREN);
            SetRegister<1>((edgeTypes & Edge::FallingAsync) >> 5, bcm2835_periph::GPIO_AFEN);

            _activeEdgeDetection = edgeTypes;
        }

        void GPIO::clearEdgeDetection()
        {
            // Clear all detection registers
            const uint32_t * offsets[] =
            {
                bcm2835_periph::GPIO_EDS,
                bcm2835_periph::GPIO_REN,
                bcm2835_periph::GPIO_FEN,
                bcm2835_periph::GPIO_HEN,
                bcm2835_periph::GPIO_LEN,
                bcm2835_periph::GPIO_AREN,
                bcm2835_periph::GPIO_AFEN,
            };

            uint32_t shift = _id % 32;

            for (auto offset : offsets)
            {
                for (int i = 0; i < 2; ++i)
                {
                    auto addr = _gpioAddr + offset[i];
                    *addr &= ~(0x1 << shift);
                }
            }
        }

        LogicalState GPIO::waitForStateChange()
        {
            dbgAssert(_activeEdgeDetection > 0);

            // Wait for state change signal
            // TODO

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
