#pragma once

#include <array>

#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"
#include "refl-cpp/function_traits.hpp"

namespace ReflCpp {
template <typename Func_>
struct FunctionWrapper {
    using Traits = FunctionTraits<Func_>;

private:
    Func_ m_Ptr;

    template <size_t... Indices>
    [[nodiscard]]
    Result<void> CheckArguments(const ArgumentList& args) const {
        constexpr size_t arg_count = sizeof...(Indices);
        std::array<Result<void>, arg_count> args_results = {
            args[Indices].template CanGetWithError<make_rvalue_reference_t<typename Traits::template Arg<Indices>::Type>>()...
        };

        auto error_it = std::find_if(args_results.begin(), args_results.end(), [](const auto& result) {
            return result.IsError();
        });

        if (error_it != args_results.end()) {
            return { RESULT_PASS_ERROR(), *error_it };
        }

        return {};
    }

    template <size_t... Indices> requires (Traits::IsStatic && Traits::HasReturn)
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        TRY(CheckArguments<Indices...>(args));

        return Variant::Create<typename Traits::ReturnType>((m_Ptr)(
            args[Indices].template Get<make_rvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        ));
    }

    template <size_t... Indices> requires (Traits::IsStatic && !Traits::HasReturn)
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        TRY(CheckArguments<Indices...>(args));

        (m_Ptr)(
            args[Indices].template Get<make_rvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        );

        return {};
    }

    template <size_t... Indices> requires (!Traits::IsStatic && !Traits::HasRReferenceObject && Traits::HasReturn)
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        TRY(CheckArguments<Indices...>(args));

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>;

        return Variant::Create<typename Traits::ReturnType>((TRY(obj.Get<ClassT_>()).*m_Ptr)(
            args[Indices].template Get<make_rvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        ));
    }

    template <size_t... Indices> requires (!Traits::IsStatic && !Traits::HasRReferenceObject && !Traits::HasReturn)
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        TRY(CheckArguments<Indices...>(args));

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>;

        (TRY(obj.Get<ClassT_>()).*m_Ptr)(
            args[Indices].template Get<make_rvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        );

        return {};
    }

    template <size_t... Indices> requires (!Traits::IsStatic && Traits::HasRReferenceObject && Traits::HasReturn)
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        TRY(CheckArguments<Indices...>(args));

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&&, typename Traits::ClassType&&>;
        
        return Variant::Create<typename Traits::ReturnType>((std::move(TRY(obj.Get<ClassT_>())).*m_Ptr)(
            args[Indices].template Get<make_rvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
        ));
    }

    template <size_t... Indices> requires (!Traits::IsStatic && Traits::HasRReferenceObject && !Traits::HasReturn)
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        TRY(CheckArguments<Indices...>(args));

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&&, typename Traits::ClassType&&>;

        (std::move(TRY(obj.Get<ClassT_>())).*m_Ptr)(
            args[Indices].template Get<make_rvalue_reference_t<typename Traits::template Arg<Indices>::Type>>().Value()...
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
