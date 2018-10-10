//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "hw/fan.h"
#include "logging.h"

#include "catch.hpp"
#include "wiringPiMock.hpp"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

using hw::Fan;

//==============================================================================
SCENARIO("Application can read a fan's current speed in RPM", "[fan][hw]")
{
    // Disable logger
    g_logger.setVerbosity(Logger::Unattainable);
}

SCENARIO("Application can control a fan's current speed in RPM", "[fan][hw]")
{
    // Disable logger
    g_logger.setVerbosity(Logger::Unattainable);
}
