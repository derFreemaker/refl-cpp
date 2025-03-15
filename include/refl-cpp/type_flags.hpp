#pragma once

#include "refl-cpp/common/flags.hpp"

namespace ReflCpp {

DEFINE_FLAGS(TypeFlags, uint8_t,
    NONE = 0,
    IsArray = 1 << 0,
    IsPointer = 1 << 1,
    IsReference = 1 << 2,
    IsConst = 1 << 3,
    IsVolatile = 1 << 3,
)

}
