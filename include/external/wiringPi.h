//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#ifdef HAS_WIRINGPI
#include <wiringPi.h>
#else
#include <chrono>
#endif

namespace beewatch
{
    namespace external
    {

        //================================================================
        /**
         * @interface IWiringPi
         */
        class IWiringPi
        {
        protected:
            /**
             * @brief Construct interface and set constant values
             */
            IWiringPi(int low, int high,
                      int input, int output, int pwmToneOutput, int gpioClock,
                      int pwmModeMs, int pwmModeBal,
                      int pudOff, int pudUp, int pudDown,
                      int intEdgeSetup, int intEdgeRising, int intEdgeFalling, int intEdgeBoth);

        public:
            //================================================================
            // Logical states
            const int c_low = 0;
            const int c_high = 1;

            // Pin modes
            const int c_input = 0;
            const int c_output = 1;
            const int c_pwmToneOutput = 2;
            const int c_gpioClock = 3;

            // PWM modes
            const int c_pwmModeMs = 0;
            const int c_pwmModeBal = 1;

            // Resistor modes
            const int c_pudOff = 0;
            const int c_pudUp = 1;
            const int c_pudDown = 2;

            // Edge detection modes
            const int c_intEdgeSetup = 0;
            const int c_intEdgeRising = 1;
            const int c_intEdgeFalling = 2;
            const int c_intEdgeBoth = 3;

            //================================================================
            IWiringPi() = default;
            virtual ~IWiringPi() = default;

            //================================================================
            virtual int digitalRead(int pin) = 0;
            virtual void digitalWrite(int pin, int value) = 0;

            virtual void pwmWrite(int pin, int value) = 0;

            //================================================================
            virtual void pinMode(int pin, int mode) = 0;
            virtual void pullUpDnControl(int pin, int pud) = 0;

            virtual void pwmSetMode(int mode) = 0;
            virtual void pwmSetRange(unsigned int range) = 0;
            virtual void pwmSetClock(int divisor) = 0;

            //================================================================
            virtual int setISR(int pin, int edgeType, void(*function)(void)) = 0;

            //================================================================
            virtual void delay(int s) = 0;
            virtual void delayMicroseconds(int us) = 0;

            virtual unsigned int micros(void) = 0;
        };

        //================================================================
        /**
         * @class WiringPi
         *
         * Implements the wiringPi library interface
         */
        class WiringPi : public IWiringPi
        {
            //================================================================
            WiringPi();

#ifndef HAS_WIRINGPI
            //================================================================
            std::chrono::steady_clock::time_point _startTime;
#endif

        public:
            //================================================================
            static WiringPi& getInstance();

            virtual ~WiringPi() = default;

            //================================================================
            virtual int digitalRead(int pin) override;
            virtual void digitalWrite(int pin, int value) override;

            virtual void pwmWrite(int pin, int value) override;

            //================================================================
            virtual void pinMode(int pin, int mode) override;
            virtual void pullUpDnControl(int pin, int pud) override;

            virtual void pwmSetMode(int mode) override;
            virtual void pwmSetRange(unsigned int range) override;
            virtual void pwmSetClock(int divisor) override;

            //================================================================
            virtual int setISR(int pin, int edgeType, void(*function)(void)) override;

            //================================================================
            virtual void delay(int s) override;
            virtual void delayMicroseconds(int us) override;

            virtual unsigned int micros(void) override;
        };

        #define wiringPi WiringPi::getInstance()
    }

    using external::WiringPi;
}