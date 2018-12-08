//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "hw/hx711.h"

#include "util/logging.h"

#include "catch.hpp"
#include "wiringPiMock.hpp"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

using io::GPIO;
using hw::HX711;

//==============================================================================
SCENARIO("Application can communicate with a mocked HX711 sensor", "[hx711][hw]")
{
    // Disable logger
    g_logger.setVerbosity(Logger::Unattainable);
}
