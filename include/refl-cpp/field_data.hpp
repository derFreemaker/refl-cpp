#pragma once

namespace ReflCpp {
template <typename T>
struct FieldData {
    T ptr;

    const char* name;
};
}
