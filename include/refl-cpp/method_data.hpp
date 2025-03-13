#pragma once

#include <vector>
#include <type_traits>

namespace ReflCpp {
template <typename Func_>
    requires std::is_member_function_pointer_v<Func_>
struct MethodData {
    Func_ ptr;

    const char* name = "$NONE$";
    std::vector<const char*> arguments{};
};
}
