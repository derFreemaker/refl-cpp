#pragma once

// #if !defined(NDEBUG) && !defined(REFLCPP_ENABLE_STACK_TRACING_ERROR)
// #define REFLCPP_ENABLE_STACK_TRACING_ERROR 1
// #endif
//
// #if REFLCPP_ENABLE_STACK_TRACING_ERROR == 0
// #undef REFLCPP_ENABLE_STACK_TRACING_ERROR
// #endif

namespace ReflCpp::detail {
struct ErrorTag {};

struct PassErrorTag {};

inline constexpr ErrorTag Error{};
inline constexpr PassErrorTag PassError{};
}