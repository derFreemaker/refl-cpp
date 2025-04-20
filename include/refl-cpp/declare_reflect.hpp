#pragma once

namespace ReflCpp {
struct Type;
struct TypeID;

template <typename T>
[[nodiscard]]
const Type& Reflect();

template <typename T>
[[nodiscard]]
TypeID ReflectID();

namespace detail {
[[nodiscard]]
const Type& Reflect(TypeID id);
}
}
