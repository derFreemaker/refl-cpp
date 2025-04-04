#pragma once

#include <memory>
#include <type_traits>

namespace ReflCpp {

template <typename T_>
struct reference_wrapper;

template <typename T_>
struct reference_wrapper<T_&> {
private:
    T_* data_;

public:
    constexpr reference_wrapper(T_& data) noexcept
        : data_(std::addressof(data)) {}

    constexpr T_& get() const noexcept {
        return static_cast<T_&>(*data_);
    }

    constexpr operator T_&() const noexcept {
        return get();
    }
};

template <typename T_>
struct reference_wrapper<T_&&> {
private:
    T_* data_;

public:
    constexpr reference_wrapper(T_&& data) noexcept
        : data_(std::addressof(data)) {}

    constexpr T_&& get() const noexcept {
        return static_cast<T_&&>(*data_);
    }

    constexpr operator T_&&() const noexcept {
        return get();
    }
};

}
