//==============================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//==============================================================================

#include "util/string.h"

#include <algorithm>

namespace beewatch::string
{

    //==============================================================================
    std::string tolower(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }

    std::string toupper(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    }

} // namespace beewatch::string
