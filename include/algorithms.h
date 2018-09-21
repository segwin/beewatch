//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <algorithm>

namespace beewatch
{

    //================================================================
    /**
     * @brief Returns true if val is found in given range
     * 
     * @param [in] first    Iterator to first element in range
     * @param [in] last     Iterator to last element in range (typically container.end())
     * @param [in] val      Value to search for
     *
     * @returns True if val is found, false otherwise
     */
    template <typename InputIterator, typename Value>
    constexpr bool contains(InputIterator first, InputIterator last, const Value& val)
    {
        return std::find(first, last, val) != last;
    }

} // namespace beewatch
