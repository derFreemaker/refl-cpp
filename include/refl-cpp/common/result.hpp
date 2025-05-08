#pragma once

#include <res-cpp/res-cpp.hpp>

namespace ReflCpp {

#define TRY(...) \
    RESCPP_TRY(__VA_ARGS__)

#define TRY_(...) \
    RESCPP_TRY_(__VA_ARGS__)

}
