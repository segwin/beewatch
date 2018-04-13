//============================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//============================================================

#pragma once

#include "io.h"

namespace beewatch::io
{

    //============================================================
    /**
     * @class Tachometer
     *
     * Models a tachometer interface
     */
    class TachometerIn : public Input<double>
    {
    public:
        //============================================================
        TachometerIn(Input<LogicalState>::Ptr inputInterface);
        virtual ~TachometerIn();

        //============================================================
        using Ptr = std::shared_ptr<TachometerIn>;

        //============================================================
        virtual double Recv();

    protected:
        //============================================================
        Input<LogicalState>::Ptr _inputInterface;
    };

}
