//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include <memory>
#include <mutex>

namespace beewatch
{
    namespace io
    {

        //==============================================================================
        /**
         * @class Input
         *
         * Models a generic input port
         */
        template <typename T>
        class Input
        {
        public:
            //==============================================================================
            virtual T read() = 0;

        protected:
            //==============================================================================
            mutable std::mutex _readMutex;
        };


        //==============================================================================
        /**
         * @class Output
         *
         * Models a generic output port
         */
        template <typename T>
        class Output
        {
        public:
            //==============================================================================
            virtual void write(T) = 0;

        protected:
            //==============================================================================
            mutable std::mutex _writeMutex;
        };


        //==============================================================================
        /**
         * @class InputOutput
         *
         * Models a generic input-output port
         */
        template <typename T>
        class InputOutput : public Input<T>, public Output<T>
        {
        };

    } // namespace io
} // namespace beewatch
