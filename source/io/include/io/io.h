//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <io/io_api.h>

#include <memory>

namespace beewatch
{
    namespace io
    {

        //================================================================
        /**
         * @class Input
         *
         * Models a generic input port
         */
        template <typename T>
        class IO_API Input
        {
        public:
            //================================================================
            using Ptr = std::shared_ptr<Input<T>>;

            //================================================================
            virtual T read() = 0;
        };


        //================================================================
        /**
         * @class Output
         *
         * Models a generic output port
         */
        template <typename T>
        class IO_API Output
        {
        public:
            //================================================================
            using Ptr = std::shared_ptr<Output<T>>;

            //================================================================
            virtual void write(T) = 0;
        };


        //================================================================
        /**
         * @class InputOutput
         *
         * Models a generic input-output port
         */
        template <typename T>
        class IO_API InputOutput : public Input<T>, public Output<T>
        {
        public:
            //================================================================
            using Ptr = std::shared_ptr<InputOutput<T>>;
        };


        //================================================================
        /**
         * @enum LogicalState
         *
         * Models the logical state on a given IO port (HI/LO)
         */
        enum class IO_API LogicalState
        {
            LO = 0,
            HI = 1,
            INVALID = -1
        };

    } // namespace io
} // namespace beewatch
