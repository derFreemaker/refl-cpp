#pragma once

namespace ReflCpp {
template <typename Derived, typename ValueT = uint32_t>
struct Flags {
protected:
    ValueT value_;

public:
    constexpr Flags() noexcept : value_(0) {}
    explicit constexpr Flags(ValueT value) noexcept : value_(value) {}

    constexpr ValueT Value() const noexcept {
        return value_;
    }

    [[nodiscard]]
    constexpr bool IsEmpty() const noexcept {
        return value_ == 0;
    }

    constexpr bool Has(Derived flag) const noexcept {
        return (value_ & flag.value_) == flag.value_;
    }

    constexpr bool Is(Derived flag) const noexcept {
        return value_ == flag.value_;
    }
    
    constexpr void Set(Derived flag) noexcept {
        value_ |= flag.value_;
    }

    constexpr void Clear(Derived flag) noexcept {
        value_ &= ~flag.value_;
    }

    constexpr void Toggle(Derived flag) noexcept {
        value_ ^= flag.value_;
    }

    constexpr Derived operator|(Derived flag) const noexcept {
        return Derived(value_ | flag.value_);
    }

    constexpr Derived operator|(const Flags& other) const noexcept {
        return Derived(value_ | other.value_);
    }

    static constexpr Derived FromValue(ValueT value) noexcept {
        return Derived(value);
    }
};

#define REFLCPP_FLAGS(NAME, UNDERLYING_TYPE, ...) \
    struct NAME : public ::ReflCpp::Flags<NAME, UNDERLYING_TYPE> { \
        enum : UNDERLYING_TYPE { __VA_ARGS__ }; \
        constexpr NAME() noexcept = default; \
        constexpr NAME(const UNDERLYING_TYPE& value) noexcept : Flags(value) {} \
    };

}
