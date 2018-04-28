//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <util/util_api.h>

namespace beewatch
{
    namespace util
    {

        //================================================================
        /**
         * @enum Priority
         *
         * Priority levels used in application
         */
        enum class UTIL_API Priority
        {
            NORMAL,
            REALTIME
        };

        //================================================================
        /**
         * @brief Set thread/process priority to highest
         */
        UTIL_API void setPriority(Priority priority);

    } // namespace util
} // namespace beewatch
