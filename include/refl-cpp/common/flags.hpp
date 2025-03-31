#pragma once

namespace ReflCpp {
template <typename Derived, typename ValueT = uint32_t>
struct Flags {
protected:
    ValueT _value;

public:
    constexpr Flags() noexcept : _value(0) {}
    explicit constexpr Flags(ValueT value) noexcept : _value(value) {}

    constexpr ValueT Value() const noexcept {
        return _value;
    }

    [[nodiscard]]
    constexpr bool IsEmpty() const noexcept {
        return _value == 0;
    }

    constexpr bool Has(Derived flag) const noexcept {
        return (_value & flag._value) == flag._value;
    }

    constexpr bool Is(Derived flag) const noexcept {
        return _value == flag._value;
    }
    
    constexpr void Set(Derived flag) noexcept {
        _value |= flag._value;
    }

    constexpr void Clear(Derived flag) noexcept {
        _value &= ~flag._value;
    }

    constexpr void Toggle(Derived flag) noexcept {
        _value ^= flag._value;
    }

    constexpr Derived operator|(Derived flag) const noexcept {
        return Derived(_value | flag._value);
    }

    constexpr Derived operator|(const Flags& other) const noexcept {
        return Derived(_value | other._value);
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
