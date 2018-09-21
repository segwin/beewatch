//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

namespace beewatch
{
    
    //================================================================
    /**
     * @struct Range
     *
     * Represents a range of values
     */
    template <typename T>
    struct Range
    {
        T min;
        T max;

        /**
         * @brief Get range midpoint
         *
         * @returns Range midpoint
         */
        inline constexpr T mid() const
        {
            return (max - min) / T(2);
        }

        /**
         * @brief Check if range contains given value
         *
         * @param [in] val  Value to be checked
         *
         * @returns True if value is in range, false otherwise
         */
        inline constexpr bool contains(T val) const
        {
            return min <= val && val <= max;
        }
    };
    
    //================================================================
    /**
     * @struct ClimateData
     *
     * Holds data related to the climate (temperature and humidity)
     */
    template <typename T>
    struct ClimateData
    {
        T humidity = T(0);
        T temperature = T(0);

        ClimateData operator+=(const ClimateData& rhs)
        {
            humidity += rhs.humidity;
            temperature += rhs.temperature;

            return *this;
        }

        friend ClimateData operator+(ClimateData lhs, const ClimateData& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        ClimateData operator-=(const ClimateData& rhs)
        {
            humidity -= rhs.humidity;
            temperature -= rhs.temperature;

            return *this;
        }

        friend ClimateData operator-(ClimateData lhs, const ClimateData& rhs)
        {
            lhs -= rhs;
            return lhs;
        }

        template <typename Divisor>
        ClimateData operator/=(const Divisor& rhs)
        {
            humidity /= rhs;
            temperature /= rhs;

            return *this;
        }

        template <typename Divisor>
        friend ClimateData operator/(ClimateData lhs, const Divisor& rhs)
        {
            lhs /= rhs;
            return lhs;
        }

        template <typename Multiplier>
        ClimateData operator*=(const Multiplier& rhs)
        {
            humidity *= rhs;
            temperature *= rhs;

            return *this;
        }

        template <typename Multiplier>
        friend ClimateData operator*(ClimateData lhs, const Multiplier& rhs)
        {
            lhs *= rhs;
            return lhs;
        }
    };

} // namespace beewatch
