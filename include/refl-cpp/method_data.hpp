#pragma once

#include <vector>

#include "refl-cpp/argument.hpp"
#include "refl-cpp/method_wrapper.hpp"

namespace ReflCpp {
template <typename Func_>
struct MethodData {
    const char* name = "$NONE$";
    Func_ func;

    std::vector<ArgumentInfo> arguments;
};
}
