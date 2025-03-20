#pragma once

#include <stdexcept>

#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"
#include "refl-cpp/function_traits.hpp"

namespace ReflCpp {
template <typename Func_>
struct FunctionWrapper {
    using Traits = FunctionTraits<Func_>;

private:
    Func_ m_Ptr;

#define INVOKE_IMPL(_TEMPLATE, _INVOKE, ...) \
    template <size_t... Indices> \
        requires (_TEMPLATE && Traits::HasReturn) \
    Variant InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...> __VA_OPT__(, ) __VA_ARGS__) const { \
        return Variant((_INVOKE)( \
            args[Indices].GetRef < typename Traits::template Arg<Indices>::Type > ()... \
        )); \
    } \
    template <size_t... Indices> \
        requires (_TEMPLATE && !Traits::HasReturn) \
    void InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...> __VA_OPT__(, ) __VA_ARGS__) const { \
        (_INVOKE)( \
            args[Indices].GetRef < typename Traits::template Arg<Indices>::Type > ()... \
        ); \
    }

    // TODO: check if we have to care about Traits::IsConst

    //NOTE: This works even if it says 'expected expression' in Clion for example
    INVOKE_IMPL(Traits::IsStatic, m_Ptr)
    INVOKE_IMPL(!Traits::IsStatic && !Traits::HasReferenceObject, obj.GetRef<typename Traits::ClassType>().*m_Ptr, const Variant& obj)
    INVOKE_IMPL(!Traits::IsStatic && Traits::HasReferenceObject && !Traits::HasRReferenceObject, obj.GetRef<typename Traits::ClassType>().*m_Ptr, const Variant& obj)
    INVOKE_IMPL(!Traits::IsStatic && Traits::HasReferenceObject && !Traits::HasLReferenceObject, std::move(obj.GetRef<typename Traits::ClassType>()).*m_Ptr, const Variant& obj)

public:
    FunctionWrapper(Func_ ptr)
        : m_Ptr(ptr) {}

    [[nodiscard]]
    constexpr bool IsStatic() const {
        return Traits::IsStatic;
    }

    [[nodiscard]]
    constexpr bool IsConst() const {
        return Traits::IsConst;
    }
    
    [[nodiscard]]
    constexpr bool HasLReferenceObject() const {
        return Traits::HasLReferenceObject;
    }
    
    [[nodiscard]]
    constexpr bool HasRReferenceObject() const {
        return Traits::HasRReferenceObject;
    }
    
    [[nodiscard]]
    Variant Invoke(const ArgumentList& args, const Variant& obj) const {
        if (args.size() != Traits::ArgCount) {
            throw std::invalid_argument("incorrect number of arguments");
        }

        if constexpr (Traits::IsStatic) {
            if constexpr (Traits::HasReturn) {
                return InvokeImpl(args, std::make_index_sequence<FunctionTraits<Func_>::ArgCount>());
            }
            else {
                InvokeImpl(args, std::make_index_sequence<FunctionTraits<Func_>::ArgCount>());
                return Variant::Void();
            }
        }
        else {
            if constexpr (Traits::HasReturn) {
                return InvokeImpl(args, std::make_index_sequence<FunctionTraits<Func_>::ArgCount>(), obj);
            }
            else {
                InvokeImpl(args, std::make_index_sequence<FunctionTraits<Func_>::ArgCount>(), obj);
                return Variant::Void();
            }
        }
    }
};
}
