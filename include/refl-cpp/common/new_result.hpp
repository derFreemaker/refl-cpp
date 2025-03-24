#pragma once

#include <optional>

#include "refl-cpp/common/stack_tracing_error.hpp"

namespace ReflCpp {
namespace detail {
struct ErrorTag {};

struct OkTag {};

inline constexpr ErrorTag Error{};
inline constexpr OkTag Ok{};

struct ResultBase {
    const std::optional<ResultError> m_Error; 
};
}
}
