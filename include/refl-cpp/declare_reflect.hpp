#pragma once

#include <vector>

#include "refl-cpp/common/result.hpp"

namespace ReflCpp {
struct Type;
struct TypeID;

template <typename T>
[[nodiscard]]
Result<const Type&> Reflect();

template <typename T>
[[nodiscard]]
Result<TypeID> ReflectID();

namespace detail {
[[nodiscard]]
Result<const Type&> Reflect(TypeID id);
}
}
