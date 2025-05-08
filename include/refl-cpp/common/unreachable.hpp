#pragma once

#include <print>
#include <stacktrace>

namespace ReflCpp {
/// Used to mark code branches as unreachable.
/// 'unreachable()' will mark the code branch as compile-time unreachable
/// meaning it should not even exist in the compiled binary.
/// While 'unreachable<true>()' allows it to exist but aborts
/// when being executed.
template <bool allow_exist = false>
    requires (!allow_exist)
[[noreturn]]
void unreachable() noexcept {
    static_assert(allow_exist, "should NOT be reachable");
}

/// Used to mark code branches as unreachable.
/// 'unreachable()' will mark the code branch as compile-time unreachable
/// meaning it should not even exist in the compiled binary.
/// While 'unreachable<true>()' allows it to exist but aborts
/// when being executed.
template <bool allow_exist>
    requires (allow_exist)
[[noreturn]]
void unreachable() noexcept {
    std::println("should NOT be reachable:\n{}",
                 std::to_string(std::stacktrace::current(1))
    );
    std::abort();
}
}
