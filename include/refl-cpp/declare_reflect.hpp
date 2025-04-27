#pragma once

#include "refl-cpp/common/result.hpp"

namespace ReflCpp {
struct Type;
struct TypeID;

enum class ReflectError : uint8_t {
    MaxLimitReached,
    CreationFailed,
    OutOfMemory,
};

template <typename T>
[[nodiscard]]
rescpp::result<const Type&, ReflectError> Reflect() noexcept;

template <typename T>
[[nodiscard]]
rescpp::result<TypeID, ReflectError> ReflectID() noexcept;

enum class GetTypeError {
    None,
    InvalidID,
    NotFound,
};

namespace detail {
[[nodiscard]]
rescpp::result<const Type&, GetTypeError> Reflect(TypeID id) noexcept;
}
}
