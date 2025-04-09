#pragma once

#include "refl-cpp/method_data.hpp"
#include "refl-cpp/method.hpp"

namespace ReflCpp {
inline MethodData::operator Method() const {
    return Method(*this);
}

}