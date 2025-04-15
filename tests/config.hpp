#pragma once

#include <gtest/gtest.h>
#include "refl-cpp/refl-cpp.hpp"

#define FAIL_TRY(...) \
    TRY_IMPL((__VA_ARGS__), { \
        FAIL() << __result__.error().str(); \
    })