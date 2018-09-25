//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "io/pwm.h"
#include "algorithms.h"
#include "logging.h"

#include "catch.hpp"
#include "wiringPiMock.hpp"

#include <cstdlib>
#include <ctime>

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

using namespace beewatch;

using io::PWM;
using io::GPIO;

//==============================================================================
SCENARIO("Application can communicate with a mocked HX711 sensor", "[hw]")
{
    // Disable non-fatal logs
    logger.setVerbosity(Logger::Fatal);
}