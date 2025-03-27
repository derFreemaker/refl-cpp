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
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...> __VA_OPT__(, ) __VA_ARGS__) const { \
        return { RESULT_OK(), Variant((_INVOKE)( \
            args[Indices].template GetRef<typename Traits::template Arg<Indices>::Type>().Value()... \
        )) }; \
    } \
    template <size_t... Indices> \
        requires (_TEMPLATE && !Traits::HasReturn) \
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...> __VA_OPT__(, ) __VA_ARGS__) const { \
        (_INVOKE)( \
            args[Indices].template GetRef<typename Traits::template Arg<Indices>::Type>().Value()... \
        ); \
        return { RESULT_OK() }; \
    }

    // TODO: check if we have to care about Traits::IsConst

    //NOTE: This works even if it says 'expected expression' from the LSP in Clion for example

    template <size_t... Indices> requires (Traits::IsStatic && Traits::HasReturn)
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        return (m_Ptr)(
            args[Indices].template Get<make_lvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        );
    }

    template <size_t... Indices> requires (Traits::IsStatic && !Traits::HasReturn)
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        (m_Ptr)(
            args[Indices].template Get<make_lvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        );
        return {};
    }

    template <size_t... Indices> requires (!Traits::IsStatic && !Traits::HasRReferenceObject && Traits::HasReturn)
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        return (TRY(obj.Get<std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>>()).*m_Ptr)(
            args[Indices].template Get<make_lvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        );
    }

    template <size_t... Indices> requires (!Traits::IsStatic && !Traits::HasRReferenceObject && !Traits::HasReturn)
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        (TRY(obj.Get<std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>>()).*m_Ptr)(
            args[Indices].template Get<make_lvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        );
        return {};
    }

    template <size_t... Indices> requires (!Traits::IsStatic && Traits::HasRReferenceObject && Traits::HasReturn)
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        return (std::move(TRY(obj.Get<std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>>())).*m_Ptr)(
            args[Indices].template Get<make_lvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        );
    }

    template <size_t... Indices> requires (!Traits::IsStatic && Traits::HasRReferenceObject && !Traits::HasReturn)
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        (std::move(TRY(obj.Get<std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>>())).*m_Ptr)(
            args[Indices].template Get<make_lvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        );
        return {};
    }

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
    Result<Variant> Invoke(const ArgumentList& args, const Variant& obj) const {
        if (args.size() != Traits::ArgCount) {
            throw std::invalid_argument("incorrect number of arguments");
        }

        if constexpr (Traits::IsStatic) {
            if constexpr (Traits::HasReturn) {
                return InvokeImpl(args, std::make_index_sequence<FunctionTraits<Func_>::ArgCount>());
            }
            else {
                TRY(InvokeImpl(args, std::make_index_sequence<FunctionTraits<Func_>::ArgCount>()));
                return Variant::Void();
            }
        }
        else {
            if constexpr (Traits::HasReturn) {
                return InvokeImpl(args, std::make_index_sequence<FunctionTraits<Func_>::ArgCount>(), obj);
            }
            else {
                TRY(InvokeImpl(args, std::make_index_sequence<FunctionTraits<Func_>::ArgCount>(), obj));
                return Variant::Void();
            }
        }
    }
};
}
