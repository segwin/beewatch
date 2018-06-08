//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <util/util_api.h>

#include <cassert>

namespace beewatch
{

    static constexpr UTIL_API void dbgAssert(int expr) { assert(expr); }

} // namespace beewatch

