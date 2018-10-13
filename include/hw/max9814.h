//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "io/io.h"
#include "io/gpio.h"

#include "patterns.h"

#include <memory>

namespace beewatch::hw
{

    //==============================================================================
    /**
     * @class MAX9814
     *
     * Models a MAX9814 audio amplifier
     */
    class MAX9814 : public unique_ownership_t<MAX9814>,
                    public io::Input<double>
    {
    public:
        //==============================================================================
        /**
         * @brief Construct MAX9814 object
         */
        MAX9814();

        //==============================================================================
        /**
         * @brief Destroy MAX9814 object
         */
        virtual ~MAX9814();
    };

} // namespace beewatch
