#pragma once

namespace ReflCpp {
template <typename T_>
struct FieldData {
    T_ ptr;

    const char* name;
};
}
