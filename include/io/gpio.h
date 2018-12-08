//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "io.h"

#include "util/patterns.hpp"
#include "external/wiringPi.h"

#include <array>
#include <atomic>
#include <bitset>
#include <map>
#include <mutex>
#include <vector>


namespace beewatch::io
{

    //==============================================================================
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

    //==============================================================================
    /**
     * @class GPIO
     *
     * Models a GPIO interface
     */
    class GPIO : public unique_ownership_t<GPIO>,
                 public InputOutput<LogicalState>
    {
    public:
        //==============================================================================
        /**
         * @brief Relinquish ownership of and destroy GPIO object
         */
        virtual ~GPIO();

        //==============================================================================
        /**
         * Number of GPIO pins on the common RPi boards (excluding compute)
         *
         * NB: This count excludes reserved GPIO pins (e.g. 5V, 3v3, GND, ...)
         */
        static constexpr int NUM_GPIO = 26;

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
         * @param [in] wiringPi wiringPi library interface (used to inject mocked class in tests)
         *
         * @returns Shared pointer to GPIO object if successful, nullptr otherwise
         */
        static GPIO::Ptr claim(int gpioId, std::shared_ptr<external::IWiringPi> wiringPi = external::WiringPi::getInstance());

        //==============================================================================
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

        //==============================================================================
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
        virtual void setMode(Mode mode);

        /**
         * @brief Get GPIO mode
         *
         * @returns Current GPIO mode
         */
        virtual Mode getMode() const;


        //==============================================================================
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
        virtual void setResistorMode(Resistor mode);

        /**
         * @brief Get resistor configuration
         *
         * @returns Current resistor configuration
         */
        virtual Resistor getResistorMode() const;

        //==============================================================================
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
        virtual void setEdgeDetection(EdgeType type, void (*callback)(void));

        /**
         * @brief Get GPIO edge detection mode
         *
         * @returns Current GPIO edge detection mode
         */
        virtual EdgeType getEdgeDetection() const;


        /**
        * @brief Disables state detection
        */
        virtual void clearEdgeDetection();

        /**
         * @brief Empty function used to clear egde detection ISR
         */
        static void nop() {}


        //==============================================================================
        /**
         * @brief Get ID of GPIO modeled by object
         *
         * @returns GPIO number
         */
        virtual int getID() { return _id; }

        //==============================================================================
        virtual std::shared_ptr<external::IWiringPi> getWiringPi() { return _wiringPi; }


    protected:
        //==============================================================================
        /**
         * @brief Construct and claim GPIO#, where # = id
         *
         * Constructor is protected and cannot be used outside of class.
         * Other classes should use the claim() factory method to try
         * and obtain a GPIO.
         *
         * @param [in] id       ID of GPIO pin to model
         * @param [in] wiringPi wiringPi library interface (used to inject mocked class in tests)
         */
        GPIO(int id, std::shared_ptr<external::IWiringPi> wiringPi);


    private:
        //==============================================================================
        std::map<LogicalState, int>   _mapLogicalStateToWiringPi;
        std::map<GPIO::Mode, int>     _mapModeToWiringPi;
        std::map<GPIO::Resistor, int> _mapResistorToWiringPi;
        std::map<GPIO::EdgeType, int> _mapEdgeTypeToWiringPi;

        //==============================================================================
        std::shared_ptr<external::IWiringPi> _wiringPi;

        //==============================================================================
        static bool _claimedGPIOList[NUM_GPIO];
        static std::mutex _claimMutex[NUM_GPIO];

        //==============================================================================
        int _id;

        Mode _mode;
        Resistor _resistorCfg;
        EdgeType _edgeDetection;
    };

} // namespace beewatch::io
