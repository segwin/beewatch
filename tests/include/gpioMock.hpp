//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "wiringPiMock.hpp"

#include "io/gpio.h"
#include "timing.h"

#include <atomic>
#include <cassert>
#include <thread>

//==============================================================================
class GPIOMocked final : public beewatch::io::GPIO
{
public:
    //==============================================================================
    using LogicalState = beewatch::io::LogicalState;
    using IWiringPi = beewatch::external::IWiringPi;

    //==============================================================================
    virtual ~GPIOMocked() = default;

    static GPIO::Ptr claim( int gpioId, std::shared_ptr<IWiringPi> wiringPi = std::make_shared<WiringPiMocked>() )
    {
        return GPIO::Ptr(new GPIOMocked(gpioId, wiringPi));
    }


    //==============================================================================
    LogicalState lastState;

    virtual void write(LogicalState state) override { lastState = state; }
    virtual LogicalState read() override { return lastState; }

    //==============================================================================
    double edgeSimFrequencyHz;      // Frequency of simulated cycle
    double edgeSimDutyCycle;        // Ratio of time signal is HI
    double edgeSimCycleOffsetRatio; // Offset of cycle start (only matters for 1st cycle)

    int edgeSimTriggerCount;        // Number of times edge detection triggered callback

    virtual void setEdgeDetection(EdgeType type, void (*callback)(void)) override
    {
        assert(!edgeDetectThread);

        stopEdgeDetect = false;
        edgeSimTriggerCount = 0;

        edgeDetectThread.reset(new std::thread(&GPIOMocked::edgeDetectWorker, this, type, callback));
    }

    virtual void clearEdgeDetection() override
    {
        stopEdgeDetect = true;
        edgeDetectThread->join();
        edgeDetectThread.reset();
    }

private:
    //==============================================================================
    GPIOMocked(int id, std::shared_ptr<IWiringPi> wiringPi) : GPIO(id, wiringPi) {}
    
    //==============================================================================
    std::unique_ptr<std::thread> edgeDetectThread;
    std::atomic<bool> stopEdgeDetect;
    
    void edgeDetectWorker(EdgeType type, void (*callback)(void))
    {
        using namespace beewatch;

        const double edgeSimPeriodMs = 1e3 / edgeSimFrequencyHz;

        // Wait for given fraction of a cycle (used to test various offsets)
        g_time.wait(edgeSimPeriodMs * edgeSimCycleOffsetRatio);

        if (type == EdgeType::None)
        {
            // Callback is never triggered
            while (!stopEdgeDetect);
        }
        else if (type == EdgeType::Both)
        {
            const double waitHiMs = edgeSimPeriodMs * edgeSimDutyCycle;
            const double waitLoMs = edgeSimPeriodMs * (1 - edgeSimDutyCycle);

            // Callback is triggered twice per cycle
            while (!stopEdgeDetect)
            {
                callback();
                edgeSimTriggerCount++;
                g_time.wait(waitLoMs);

                if (stopEdgeDetect)
                    break;

                callback();
                edgeSimTriggerCount++;
                g_time.wait(waitHiMs);
            }
        }
        else    // EdgeType::Rising or EdgeType::Falling
        {
            // Callback is triggered once per cycle
            while (!stopEdgeDetect)
            {
                callback();
                edgeSimTriggerCount++;
                g_time.wait(edgeSimPeriodMs);
            }
        }
    }
};
