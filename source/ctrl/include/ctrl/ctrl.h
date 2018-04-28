//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <ctrl/ctrl_api.h>

#include <hw/dht11.h>
#include <hw/fan.h>

#include <memory>

namespace beewatch
{
    namespace ctrl
    {

        //================================================================
        /**
         * @class FanCtrl
         *
         * Controls a Fan object based on current temperature and humidity
         * readings
         */
        class CTRL_API FanCtrl
        {
        public:

        };

    } // namespace climate
} // namespace beewatch
