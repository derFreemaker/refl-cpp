#pragma once
#if defined(__cpp_lib_stacktrace)
#if !defined(NDEBUG) && !defined(REFLCPP_ENABLE_STACK_TRACING_ERROR)
#define REFLCPP_ENABLE_STACK_TRACING_ERROR 1
#endif
#else
#define REFLCPP_ENABLE_STACK_TRACING_ERROR 0
#endif

namespace ReflCpp::detail {
struct ErrorTag {};

struct PassErrorTag {};

inline constexpr ErrorTag Error{};
inline constexpr PassErrorTag PassError{};
}
