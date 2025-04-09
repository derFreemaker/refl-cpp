#pragma once

#include <vector>

#include "refl-cpp/method_func.hpp"

namespace ReflCpp {
struct Method;

struct MethodData {
    const char* name = "$NONE$";
    std::vector<std::unique_ptr<MethodFunc>> funcs;
};
}
