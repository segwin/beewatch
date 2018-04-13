//============================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//============================================================

#pragma once

#include <memory>

namespace beewatch::io
{

    //============================================================
    /**
     * @class Input
     *
     * Models a generic input port
     */
    template <typename T>
    class Input
    {
    public:
        //============================================================
        using Ptr = std::shared_ptr<Input<T>>;

        //============================================================
        virtual T Recv() = 0;
    };


    //============================================================
    /**
     * @class Output
     *
     * Models a generic output port
     */
    template <typename T>
    class Output
    {
    public:
        //============================================================
        using Ptr = std::shared_ptr<Output<T>>;

        //============================================================
        virtual void Send(T) = 0;
    };


    //============================================================
    /**
     * @class InputOutput
     *
     * Models a generic input-output port
     */
    template <typename T>
    class InputOutput : public Input<T>, public Output<T>
    {
    public:
        //============================================================
        using Ptr = std::shared_ptr<InputOutput<T>>;
    };


    //============================================================
    /**
     * @enum LogicalState
     *
     * Models the logical state on a given IO port (HI/LO)
     */
    enum class LogicalState
    {
        LO = 0,
        HI = 1
    };

}
