//============================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//============================================================

#pragma once

#include "io.h"

namespace beewatch::io
{

    //============================================================
    /**
    * @class GPIO
    *
    * Models a GPIO port
    */
    class GPIO : public InputOutput<LogicalState>
    {
    public:
        //============================================================
        /**
         * @brief Construct and export GPIO#, where # = id
         *
         * @param in id   ID of GPIO pin to model
         */
        GPIO(const unsigned id);

        /**
         * @brief Unexport and destroy GPIO object
         */
        virtual ~GPIO();

        //============================================================
        using Ptr = std::shared_ptr<GPIO>;

        //============================================================
        /**
         * @enum GPIO direction
         */
        enum Direction
        {
            OUT,
            IN
        };

        /**
         * @brief Set GPIO direction
         *
         * @param [in] direction  Direction to use on GPIO
         */
        virtual void SetDirection(Direction direction);

        /**
         * @brief Get current GPIO direction
         *
         * @returns Current GPIO direction
         */
        virtual Direction GetDirection();

        //============================================================
        /**
         * @brief Poll current state (HI/LO) on GPIO
         *
         * @returns Current GPIO state
         */
        virtual LogicalState Recv();

        /**
         * @brief Write state (HI/LO) to GPIO
         *
         * @param [in] state    State to write to GPIO
         */
        virtual void Send(LogicalState state);

        //============================================================
        /**
         * @brief Get ID of GPIO modeled by object
         *
         * @returns GPIO number
         */
        unsigned GetId() { return _id; }

    protected:
        //============================================================
        unsigned _id;
    };

}