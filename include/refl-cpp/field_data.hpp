#pragma once

#include <type_traits>

namespace ReflCpp {
template <typename T_>
    requires std::is_member_pointer_v<T_>
struct FieldData {
    T_ ptr;

    const char* name = "$NONE$";
};
}
