//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "hw/dhtxx.h"
#include "logging.h"

#include "catch.hpp"
#include "wiringPiMock.hpp"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

using hw::DHTxx;

//==============================================================================
class DHT11Mocked : public WiringPiMocked
{
};

SCENARIO("Application can communicate with a mocked DHT11 sensor", "[hw]")
{
    // Disable logger
    logger.setVerbosity(Logger::Unattainable);
}