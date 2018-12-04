//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "io/tachometer.h"
#include "timing.h"

#include <chrono>

namespace beewatch::io
{

    //==============================================================================
    Tachometer::Tachometer(GPIO::Ptr&& gpio, int pulsesPerRevolution)
        : _pulsesPerRevolution(pulsesPerRevolution)
    {
        if (!gpio)
        {
            throw std::invalid_argument("Received undefined GPIO");
        }

        // Take ownership of GPIO
        _gpio = std::move(gpio);

        // Configure GPIO
        _gpio->setMode(GPIO::Mode::Input);
    }

    Tachometer::~Tachometer() = default;

    //==============================================================================
    static int edgeCount;

    static void incrementPulseCount()
    {
        edgeCount++;
    }

    double Tachometer::read()
    {
        std::lock_guard<std::mutex> guard(_readMutex);

        edgeCount = 0;
        _gpio->setEdgeDetection(GPIO::EdgeType::Both, &incrementPulseCount);

        // Wait for 500 ms to detect down to 2 pulse/s (1 Hz if nb. pulses per revolution = 2)
        g_timeRaw.wait(c_readTimeMs);

        _gpio->clearEdgeDetection();

        double nbRevolutions = (edgeCount / 2.0) / _pulsesPerRevolution;
        _speedRpm = 60 * nbRevolutions / (c_readTimeMs / 1e3);

        return _speedRpm;
    }

} // namespace beewatch::io
