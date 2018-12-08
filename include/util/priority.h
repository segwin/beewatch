//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "util/patterns.hpp"

namespace beewatch
{
    
    //==============================================================================
    /**
     * @enum Priority
     *
     * Priority levels supported by application.
     */
    enum class Priority : int
    {
        Normal,
        Background,
        RealTime
    };

    //==============================================================================
    /**
     * @class PriorityGuard
     *
     * Sets thread priority using RAII semantics to manage priority settings
     * with a fixed lifetime.
     */
    class PriorityGuard final : public unique_ownership_t<PriorityGuard>
    {
    public:
        //==============================================================================
        /**
         * @brief Constructs PriorityGuard, setting priority to given level
         */
        PriorityGuard(Priority lvl);

        /**
         * @brief Destroys PriorityGuard, restoring previous priority level
         */
        ~PriorityGuard();

    private:
        //==============================================================================
        /**
         * @brief Set process & scheduling parameters according to given priority
         *
         * @param [in] lvl  Priority level to use
         */
        void setPriority(Priority lvl);

        /**
         * @brief Read nice value to deduce current priority level
         *
         * Ignores thread scheduling policy & priority.
         *
         * @returns Current priority level
         */
        Priority getPriority();

        //==============================================================================
        Priority _prevLvl;
    };

} // namespace beewatch
