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
         * @class HX711
         *
         * Models a HX711 ADC for load cell measurements
         */
        class HW_API HX711
        {
        public:
            //================================================================
            /**
             * Unique pointer to an HX711 object
             */
            using Ptr = std::unique_ptr<HX711>;

            //================================================================
            /**
             * @brief Construct HX711 object
             */
            HX711();

            //================================================================
            /**
             * @brief Destroy HX711 object
             */
            virtual ~HX711();
        };

    } // namespace hw
} // namespace beewatch
