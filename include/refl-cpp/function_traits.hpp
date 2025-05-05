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

    template <size_t First, size_t... Rest>
    static rescpp::result<void, ReflectError> GetArgsImplImpl(std::vector<TypeID>& args) {
        args.push_back(TRY(ReflectID<typename Arg<First>::Type>()));

        if constexpr (sizeof...(Rest) == 0) {
            return {};
        }
        else {
            return GetArgsImplImpl<Rest...>(args);
        }
    }

    template <size_t... Indices>
    static rescpp::result<const std::vector<TypeID>&, ReflectError> GetArgsImpl(std::index_sequence<Indices...>) {
        static std::vector<TypeID> args{};
        if (!args.empty()) {
            return args;
        }

        args.reserve(sizeof...(Indices));
        TRY(GetArgsImplImpl<Indices...>(args));
        return args;
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

    static rescpp::result<const std::vector<TypeID>&, ReflectError> GetArgs() {
        if constexpr (ArgCount == 0) {
            static std::vector<TypeID> args{};
            return args;
        }
        else {
            return GetArgsImpl(std::make_index_sequence<ArgCount>());
        }
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
