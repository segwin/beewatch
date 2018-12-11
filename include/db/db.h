//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#pragma once

#include "patterns.hpp"

namespace beewatch::db
{

    //==============================================================================
    class DB : public unique_ownership_t<DB>
    {
    public:
        //==============================================================================
        /// Create DB object for given hostname
        DB(std::string host = "localhost");
        /// Destroy DB object
        ~DB();

        //==============================================================================

    private:
        std::string _host;
    };

} // namespace beewatch::db
