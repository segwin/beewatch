#pragma once

#include "patterns.h"

#include <map>
#include <mutex>
#include <shared_mutex>
#include <vector>

namespace beewatch
{

    //==============================================================================
    /**
     * @class BidirectionalMap
     *
     * Map of unique keys & values that can be accessed bidirectionally
     */
    template <typename T1, typename T2>
    class BidirectionalMap : public shared_ownership_t<BidirectionalMap<T1, T2>>
    {
    public:
        //==============================================================================
        /**
         * @brief Default constructor
         */
        BidirectionalMap() = default;

        /**
         * @brief Initializer list constructor
         */
        BidirectionalMap(std::initializer_list<std::pair<T1, T2>> values)
        {
            *this = values;
        }

        /**
         * @brief Destructor
         */
        virtual ~BidirectionalMap() = default;

        //==============================================================================
        /**
         * @brief Assignment operator for initializer list
         */
        BidirectionalMap& operator=(std::initializer_list<std::pair<T1, T2>> list)
        {
            clear();

            std::lock_guard<std::shared_mutex> guard(_mutex);

            for (std::pair<T1, T2> values : list)
            {
                emplace_noMutex(values.first, values.second);
            }

            return *this;
        }

        //==============================================================================
        /**
         * @brief Add element to map
         *
         * @param [in] first    Value in first set
         * @param [in] second   Value in second set
         */
        void emplace(const T1& first, const T2& second)
        {
            std::lock_guard<std::shared_mutex> guard(_mutex);
            emplace_noMutex(first, second);
        }

        /**
         * @brief Remove all elements from map
         */
        void clear()
        {
            std::lock_guard<std::shared_mutex> guard(_mutex);

            _map1stTo2nd.clear();
            _map2ndTo1st.clear();
        }

        //==============================================================================
        /**
         * @brief Get value in map using first value as key
         *
         * @param [in] lookupVal    Value to search for in first set
         *
         * @returns Corresponding value in second set
         */
        const T2& at1(const T1& lookupVal) const
        {
            std::shared_lock<std::shared_mutex> readLock(_mutex);

            auto value2nd = _map1stTo2nd.at(lookupVal);
            if (!value2nd)
            {
                throw std::runtime_error("Invalid pointer in BidirectionalMap");
            }

            return *value2nd;
        }

        /**
         * @brief Get value in map using second value as key
         *
         * @param [in] lookupVal    Value to search for in second set
         *
         * @returns Corresponding value in second set
         */
        const T1& at2(const T2& lookupVal) const
        {
            std::shared_lock<std::shared_mutex> readLock(_mutex);

            auto value1st = _map2ndTo1st.at(lookupVal);
            if (!value1st)
            {
                throw std::runtime_error("Invalid pointer in BidirectionalMap");
            }

            return *value1st;
        }

    private:
        //==============================================================================
        void emplace_noMutex(const T1& first, const T2& second)
        {
            // Ensure any values added are unique
            if ( _map1stTo2nd.find(first) != _map1stTo2nd.end() ||
                 _map2ndTo1st.find(second) != _map2ndTo1st.end() )
            {
                throw std::invalid_argument("All values in a BidirectionalMap must be unique");
            }

            // Add actual values to map
            auto emplace1st = _map1stTo2nd.emplace(first, nullptr);
            auto emplace2nd = _map2ndTo1st.emplace(second, nullptr);

            // Link elements in respective lookup maps
            _map1stTo2nd.at(first) = &emplace2nd.first->first;
            _map2ndTo1st.at(second) = &emplace1st.first->first;
        }

        //==============================================================================
        std::map<T1, const T2*> _map1stTo2nd;
        std::map<T2, const T1*> _map2ndTo1st;

        mutable std::shared_mutex _mutex;
    };

} // namespace beewatch
