#pragma once

#include <vector>

#include "refl-cpp/argument.hpp"

namespace ReflCpp {
struct MethodData {
    void* ptr;

    const char* name;
    TypeID return_type;
    std::vector<ArgumentInfo> arguments;

    bool is_static;
    bool is_const;
};
}
