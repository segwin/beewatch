//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include <algorithm>
#include <numeric>
#include <string>

namespace beewatch
{

    //==============================================================================
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
    inline constexpr bool contains(InputIterator first, InputIterator last, const Value& val)
    {
        return std::find(first, last, val) != last;
    }

    //==============================================================================
    /**
     * @brief Get average value in range
     *
     * NB: Value type must implement operator+ and operator*. We assume the value to be numeric.
     * 
     * @param [in] first    Iterator to first element in range
     * @param [in] last     Iterator to last element in range (typically container.end())
     */
    template <typename InputIterator>
    inline constexpr typename std::iterator_traits<InputIterator>::value_type average(InputIterator first, InputIterator last)
    {
        using ValueType = typename std::iterator_traits<InputIterator>::value_type;

        auto size = std::distance(first, last);
        auto result = ValueType{};
        
        for (auto it = first; it != last; ++it)
        {
            result += *it / size;
        }

        return result;
    }

    //==============================================================================
    /**
     * @brief Removes all elements in container that match the given predicate
     * 
     * @param [in] container    Container in which to filter out elements
     * @param [in] predicate    Predicate to use to filter container
     */
    template <typename Container, typename UnaryPredicate>
    inline constexpr void filter(Container& container, UnaryPredicate predicate)
    {
        container.erase(std::remove_if(container.begin(), container.end(), predicate), container.end());
    }

} // namespace beewatch
