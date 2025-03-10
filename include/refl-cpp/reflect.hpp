#pragma once

#include "refl-cpp/type.hpp"
#include "refl-cpp/reflect_data_instance.hpp"

namespace ReflCpp {
template <typename T>
const Type& Reflect() {
    return ReflectDataInstance<T>::Instance();
}

[[nodiscard]]
inline const Type& Reflect(const TypeID id) {
    return ReflectionDatabase::Instance().GetType(id);
}
}

#include "refl-cpp/meta/builtin.hpp"
