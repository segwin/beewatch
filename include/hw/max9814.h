//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include <memory>

namespace beewatch
{
    namespace hw
    {

        //==============================================================================
        /**
         * @class MAX9814
         *
         * Models a MAX9814 audio amplifier
         */
        class MAX9814
        {
        public:
            //==============================================================================
            /**
             * Unique pointer to a MAX9814 object
             */
            using Ptr = std::unique_ptr<MAX9814>;

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

    } // namespace hw
} // namespace beewatch
