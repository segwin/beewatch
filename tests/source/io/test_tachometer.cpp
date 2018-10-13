//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "io/tachometer.h"
#include "logging.h"

#include "catch.hpp"
#include "gpioMock.hpp"

#include <cstdlib>
#include <ctime>
#include <string>

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

using io::Tachometer;

//==============================================================================
SCENARIO("Create a Tachometer object to read tacho input on a GPIO", "[tachometer][io][core]")
{
    // Disable logger
    g_logger.setVerbosity(Logger::Unattainable);

    GIVEN("a mocked GPIO")
    {
        auto gpioMock = GPIOMocked::claim(12);

        WHEN("a Tachometer is constructed for this gpio")
        {
            Tachometer::Ptr tachometer = std::make_unique<Tachometer>(std::move(gpioMock));

            THEN("the mocked GPIO should be consumed by the Tachometer object")
            {
                REQUIRE(gpioMock == nullptr);
            }
        }

        static constexpr int c_pulsesPerRevolutionCases[] { 1, 2 };
        static constexpr int c_frequencyHzCases[] { 120, 29, 5 };
        static constexpr int c_dutyCyclePctCases[] { 2, 23, 67 };
        static constexpr int c_startOffsetPctCases[] { 0, 61, 97 };

        for (int pulsesPerRevolution : c_pulsesPerRevolutionCases)
        WHEN("a Tachometer is constructed with " + std::to_string(pulsesPerRevolution) + " pulses per revolution")
        {
            GPIOMocked * rawGpioPtr = dynamic_cast<GPIOMocked*>(gpioMock.get());
            Tachometer::Ptr tachometer = std::make_unique<Tachometer>(std::move(gpioMock), pulsesPerRevolution);

            for (int frequencyHz : c_frequencyHzCases)
            for (int dutyCyclePct : c_dutyCyclePctCases)
            for (int startOffsetPct : c_startOffsetPctCases)
            AND_WHEN("we target " + std::to_string(frequencyHz) + " Hz pulses "
                     "(duty cycle = " + std::to_string(dutyCyclePct) + "%, "
                      "cycle start offset = " + std::to_string(startOffsetPct) + "%)")
            {
                rawGpioPtr->edgeSimFrequencyHz = (double)frequencyHz;
                rawGpioPtr->edgeSimDutyCycle = (double)dutyCyclePct / 100.0;
                rawGpioPtr->edgeSimCycleOffsetRatio = (double)startOffsetPct / 100.0;

                const double readTimeS = Tachometer::c_readTimeMs / 1e3;
                const double expectedEdgeTransitions = readTimeS * (2 * pulsesPerRevolution * frequencyHz);

                const double readValueRpm = tachometer->read();

                int actualEdgeTransitions = rawGpioPtr->edgeSimTriggerCount;

                AND_WHEN("we achieve " + std::to_string(actualEdgeTransitions) + " edge transitions "
                         "(out of " + std::to_string((int)expectedEdgeTransitions) + " targeted)")
                {
                    const double actualFrequencyHz = (actualEdgeTransitions / (2.0 * pulsesPerRevolution)) / readTimeS;
                    const double actualFrequencyRpm = actualFrequencyHz * 60;

                    THEN("tachometer->read() should return " + std::to_string((int)actualFrequencyRpm) + " RPM")
                    {
                        REQUIRE(readValueRpm == Approx(actualFrequencyRpm));
                    }
                }
            }
        }
    }
}
