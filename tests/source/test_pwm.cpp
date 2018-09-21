//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#include "io/pwm.h"

#include "catch.hpp"

/**
 * How to write tests with Catch:
 * https://github.com/catchorg/Catch2/blob/master/docs/tutorial.md#bdd-style
 */

//==============================================================================
/**
SCENARIO( "Scenario description (e.g. 'vectors can be sized and resized')", "[Category (e.g. 'vector')]" )
{
    GIVEN( "Configuration description (e.g. 'A vector with some items')" )
    {
        // Prepare test

        REQUIRE( SomeCondition );

        WHEN( "Action 1 description (e.g. 'the size is increased')" )
        {
            // Do action

            THEN( "Expected result description (e.g. 'the size and capacity change')" )
            {
                REQUIRE( TheCondition );
            }
        }
        WHEN( "Action 2 description (e.g. 'the size is reduced')" )
        {
            // ...
        }
    }
    // ...
}
**/
