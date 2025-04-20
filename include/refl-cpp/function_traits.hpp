#pragma once

#include <vector>

#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
template <typename T>
struct FunctionTraits;

namespace detail {
template <bool IsStatic_, bool IsConst_, bool HasLReferenceObject_, bool HasRReferenceObject_, typename C, typename R, typename... Args_>
struct FunctionTraitsBase {
private:
    static constexpr size_t m_ArgCount = sizeof...(Args_);

    // If this gets triggered, then something is wrong with YOUR code.
    static_assert(m_ArgCount < 256, "only support up to 255 arguments");

    template <size_t... Indices>
    static const std::vector<TypeID>& GetArgsImpl(std::index_sequence<Indices...>) {
        static std::vector<TypeID> results{};
        if (!results.empty()) {
            return results;
        }
        
        // results.reserve(sizeof...(Indices));
        (results.push_back(ReflectID<typename Arg<Indices>::Type>()), ...);
        return results;
    }

public:
    static constexpr bool IsStatic = IsStatic_;
    static constexpr bool IsConst = IsConst_;

    static constexpr bool HasReferenceObject = HasLReferenceObject_ || HasRReferenceObject_;
    static constexpr bool HasLReferenceObject = HasLReferenceObject_;
    static constexpr bool HasRReferenceObject = HasRReferenceObject_;

    using ClassType = C;
    using ReturnType = R;
    static constexpr bool HasReturn = !std::is_same_v<R, void>;

    static constexpr uint8_t ArgCount = m_ArgCount;

    template <uint8_t I>
    struct Arg {
        using Type = std::tuple_element_t<I, std::tuple<Args_...>>;
    };

    static const std::vector<TypeID>& GetArgs() {
        return GetArgsImpl(std::make_index_sequence<ArgCount>());
    }
};
}

template <typename R, typename... Args_>
struct FunctionTraits<R(*)(Args_...)>
    : public detail::FunctionTraitsBase<true, false, false, false, void, R, Args_...> {};

template <typename C, typename R, typename... Args_>
struct FunctionTraits<R(C::*)(Args_...)>
    : public detail::FunctionTraitsBase<false, false, false, false, C, R, Args_...> {};

template <typename C, typename R, typename... Args_>
struct FunctionTraits<R(C::*)(Args_...) const>
    : public detail::FunctionTraitsBase<false, true, false, false, C, R, Args_...> {};

template <typename C, typename R, typename... Args_>
struct FunctionTraits<R(C::*)(Args_...) &>
    : public detail::FunctionTraitsBase<false, false, true, false, C, R, Args_...> {};

template <typename C, typename R, typename... Args_>
struct FunctionTraits<R(C::*)(Args_...) const &>
    : public detail::FunctionTraitsBase<false, true, true, false, C, R, Args_...> {};

template <typename C, typename R, typename... Args_>
struct FunctionTraits<R(C::*)(Args_...) &&>
    : public detail::FunctionTraitsBase<false, false, false, true, C, R, Args_...> {};

template <typename C, typename R, typename... Args_>
struct FunctionTraits<R(C::*)(Args_...) const &&>
    : public detail::FunctionTraitsBase<false, true, false, true, C, R, Args_...> {};

template <typename T>
struct FunctionTraits
    : public FunctionTraits<decltype(&T::operator())> {};
}
