//============================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//============================================================

#pragma once

#include "io.h"

namespace beewatch::io
{

    //============================================================
    /**
     * @class PWMOut
     *
     * Models a PWM output interface
     */
    class PWMOut : public Output<double>
    {
    public:
        //============================================================
        PWMOut(Output<LogicalState>::Ptr outputOutterface);
        virtual ~PWMOut();

        //============================================================
        using Ptr = std::shared_ptr<PWMOut>;

        //============================================================
        virtual void Send(double val);

    protected:
        //============================================================
        Output<LogicalState>::Ptr _outputInterface;
    };

}
