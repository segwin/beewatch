//================================================================
// Copyright (c) 2018 Eric Seguin, all rights reserved.
//================================================================

#pragma once

#include <util/util_api.h>

#include <cassert>

#ifdef DEBUG
static constexpr UTIL_API void dbgAssert(int expr) { assert(expr); }
#else
static constexpr UTIL_API void dbgAssert(int) { }
#endif

