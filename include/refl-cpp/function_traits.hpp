#pragma once

#include <tuple>

namespace ReflCpp {
template <typename T>
struct FunctionTraits;

namespace detail {
template <bool IsStatic_, bool IsConst_, typename C_, typename R_, typename... Args_>
struct FunctionTraitsBase {
private:
    static constexpr size_t m_ArgCount = sizeof...(Args_);

    // If this gets triggered, then something is wrong with YOUR code.
    static_assert(m_ArgCount < 256, "only support up to 255 arguments");

public:
    static constexpr bool IsStatic = IsStatic_;
    static constexpr bool IsConst = IsConst_;

    using ClassType = C_;
    using ReturnType = R_;

    static constexpr uint8_t ArgCount = m_ArgCount;
    using Args = std::tuple<Args_...>;

    template <uint8_t I>
    struct Arg {
        using Type = std::tuple_element_t<I, std::tuple<Args_...>>;
    };
};
}

template <typename R_, typename... Args_>
struct FunctionTraits<R_(*)(Args_...)>
    : public detail::FunctionTraitsBase<true, false, void, R_, Args_...> {};

template <typename C_, typename R_, typename... Args_>
struct FunctionTraits<R_(C_::*)(Args_...)>
    : public detail::FunctionTraitsBase<false, false, C_, R_, Args_...> {};

template <typename C_, typename R_, typename... Args_>
struct FunctionTraits<R_(C_::*)(Args_...) const>
    : public detail::FunctionTraitsBase<false, true, C_, R_, Args_...> {};

template <typename T_>
struct FunctionTraits
    : public FunctionTraits<decltype(&T_::operator())> {};
}
