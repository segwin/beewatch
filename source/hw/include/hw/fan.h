//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <hw/hw_api.h>

#include <memory>

namespace beewatch
{
    namespace hw
    {

        //================================================================
        /**
         * @class Fan
         *
         * Models a 4-pin PWM fan
         */
        class HW_API Fan
        {
        public:
            //================================================================
            /**
             * Unique pointer to a Fan object
             */
            using Ptr = std::unique_ptr<Fan>;

            //================================================================
            /**
             * @brief Construct Fan object
             */
            Fan();

            //================================================================
            /**
             * @brief Destroy Fan object
             */
            virtual ~Fan();
        };

    } // namespace hw
} // namespace beewatch
