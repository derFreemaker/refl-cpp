#pragma once

#include "refl-cpp/declare_reflect.hpp"

#include "refl-cpp/type.hpp"
#include "refl-cpp/type_instance.hpp"

namespace ReflCpp {
template <typename T>
[[nodiscard]]
Result<const Type&> Reflect() noexcept {
    return TypeInstance<T>::Type();
}

template <typename T>
[[nodiscard]]
Result<TypeID> ReflectID() noexcept {
    return TypeInstance<T>::ID();
}

namespace detail {
[[nodiscard]]
inline Result<const Type&> Reflect(const TypeID id) noexcept {
    return ReflectionDatabase::Instance().GetType(id);
}
}
}

#include "refl-cpp/meta/meta.hpp"
