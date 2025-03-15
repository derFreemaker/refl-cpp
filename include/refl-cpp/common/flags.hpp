#pragma once

#include <type_traits>

namespace ReflCpp::Common {
template <typename Derived, typename ValueT = uint32_t>
struct Flags {
protected:
    ValueT value_;

public:
    using ValueType = ValueT;

    constexpr Flags() noexcept : value_(0) {}
    explicit constexpr Flags(ValueT value) noexcept : value_(value) {}

    constexpr ValueT Value() const noexcept {
        return value_;
    }

    [[nodiscard]]
    constexpr bool IsEmpty() const noexcept {
        return value_ == 0;
    }

    constexpr bool Has(ValueT flag) const noexcept {
        return (value_ & flag) == flag;
    }

    constexpr void Set(ValueT flag) noexcept {
        value_ |= flag;
    }

    constexpr void Clear(ValueT flag) noexcept {
        value_ &= ~flag;
    }

    constexpr void Toggle(ValueT flag) noexcept {
        value_ ^= flag;
    }

    constexpr Derived operator|(ValueT flag) const noexcept {
        return Derived(value_ | flag);
    }

    constexpr Derived operator|(const Flags& other) const noexcept {
        return Derived(value_ | other.value_);
    }

    static constexpr Derived FromValue(ValueT value) noexcept {
        return Derived(value);
    }
};

#define DEFINE_FLAGS(NAME, UNDERLYING_TYPE, ...) \
    struct NAME : public ::ReflCpp::Common::Flags<NAME, UNDERLYING_TYPE> { \
        enum : UNDERLYING_TYPE { __VA_ARGS__ }; \
        constexpr NAME() noexcept = default; \
        constexpr NAME(const UNDERLYING_TYPE& value) noexcept : Flags(value) {} \
    };

}
