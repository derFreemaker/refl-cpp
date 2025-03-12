#pragma once

#include "refl-cpp/declare_reflect.hpp"

#include "refl-cpp/type.hpp"
#include "refl-cpp/reflect_data_instance.hpp"

namespace ReflCpp {

template <typename T>
[[nodiscard]]
const Type& Reflect() {
    return ReflectDataInstance<T>::Type();
}

template <typename T>
[[nodiscard]]
TypeID ReflectID() {
    return ReflectDataInstance<T>::ID();
}

[[nodiscard]]
inline const Type& Reflect(const TypeID id) {
    return ReflectionDatabase::Instance().GetType(id);
}
}

#include "refl-cpp/meta/builtin.hpp"
#include "refl-cpp/meta/std.hpp"
#include "refl-cpp/meta/common.hpp"
