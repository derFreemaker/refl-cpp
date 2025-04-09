#pragma once

#include "refl-cpp/common/result.hpp"

namespace ReflCpp {
struct Type;
struct TypeID;

template <typename T>
[[nodiscard]]
Result<const Type&> Reflect() noexcept;

template <typename T>
[[nodiscard]]
Result<TypeID> ReflectID() noexcept;

namespace detail {
[[nodiscard]]
Result<const Type&> Reflect(TypeID id) noexcept;
}
}
