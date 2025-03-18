#pragma once

#include "refl-cpp/common/flags.hpp"

namespace ReflCpp {

DEFINE_FLAGS(TypeFlags, uint8_t,
    NONE = 0,
    IsArray = 1 << 0,
    IsPointer = 1 << 1,
    IsLValueReference = 1 << 2,
    IsRValueReference = 1 << 3,
    IsConst = 1 << 4,
    IsVolatile = 1 << 5,
)

}
