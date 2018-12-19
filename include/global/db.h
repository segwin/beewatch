//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "util/data_types.hpp"
#include "util/patterns.hpp"

#include <map>

namespace beewatch
{

    //==============================================================================
    class DB : public singleton_t<DB>
    {
    public:
        //==============================================================================
        /// Create DB object for given hostname
        DB(std::string host = "localhost");
        /// Destroy DB object
        ~DB();

        //==============================================================================
        /// Read node from DB
        std::map<time_t, ClimateData<double>> readClimateData(time_t since = 0) const;

        /// 

    private:
        std::string _host;
    };

} // namespace beewatch::db
