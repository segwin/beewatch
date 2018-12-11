//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include <cmath>
#include <string>

namespace beewatch::string
{

    //==============================================================================
    /**
     * @brief Create a string representation of a number with N decimal places
     *
     * @param [in] number   Number to represent as a string
     *
     * @returns String representation of given number
     */
    template <typename T>
    static inline std::string fromNumber(T num, int decimalPlaces = 2)
    {
        int wholePart = static_cast<int>(num);

        double decimalMultiplier = std::pow(10, decimalPlaces);
        int decimalPart = static_cast<int>(decimalMultiplier * (num - wholePart));

        return std::to_string(wholePart) + "." + std::to_string(decimalPart);
    }

    //==============================================================================
    /**
     * @brief Transform string to lowercase
     *
     * @param [in] str  String to cast to lowercase
     *
     * @returns Lowercase string
     */
    std::string tolower(std::string str);

    /**
     * @brief Transform string to uppercase
     *
     * @param [in] str  String to cast to uppercase
     *
     * @returns Uppercase string
     */
    std::string toupper(std::string str);

    //==============================================================================
    std::string indent(std::string str, int indentSize = 4);

} // namespace beewatch::string
