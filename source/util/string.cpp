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

    //==============================================================================
    std::string indent(std::string str, int indentSize)
    {
        str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
        std::string indent(indentSize, ' ');

        size_t pos = 0;
        while ((pos = str.find("\n", pos)) != std::string::npos)
        {
            str.replace(pos, 1, "\n" + indent);
            pos += indent.size();
        }

        // Don't forget to indent the first line
        return indent + str;
    }

} // namespace beewatch::string
