#pragma once

#include <type_traits>

namespace ReflCpp {
template <typename T_>
struct FieldData {
    T_ ptr;

    const char* name;
};
}
