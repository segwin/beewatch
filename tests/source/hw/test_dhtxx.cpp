//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "hw/dhtxx.h"

#include "global/logging.h"

#include "catch.hpp"
#include "wiringPiMock.hpp"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

using hw::DHTxx;

//==============================================================================
SCENARIO("Application can communicate with a mocked DHT11 sensor", "[dht][hw]")
{
    // Disable logger
    g_logger.setVerbosity(Logger::Level::Unattainable);
}
