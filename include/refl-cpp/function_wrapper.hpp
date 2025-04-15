#pragma once

#include <array>

#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"
#include "refl-cpp/function_traits.hpp"

namespace ReflCpp {
template <typename T_>
struct FunctionWrapper {
private:
    using Traits = FunctionTraits<T_>;
    T_ ptr_;

    template <size_t... Indices>
    [[nodiscard]]
    Result<void> CheckArgs(const ArgumentList& args) const {
        constexpr size_t arg_count = sizeof...(Indices);
        std::array<Result<void>, arg_count> args_results = {
            args[Indices].template CanGetWithError<typename Traits::template Arg<Indices>::Type>().Hold()...
        };

        auto error_it = std::find_if(args_results.begin(), args_results.end(), [](const auto& result) {
            return result.has_error();
        });

        if (error_it != args_results.end()) {
            return *error_it;
        }

        return {};
    }

    template <size_t... Indices>
    [[nodiscard]]
    Result<void> CheckArgs(const ArgumentList& args, std::index_sequence<Indices...>) const {
        return CheckArgs<Indices...>(args);
    }

#define REFLCPP_FUNCTION_WRAPPER_GET_ARGS() \
    args[Indices].template Get<std::conditional_t< \
        std::is_pointer_v<typename Traits::template Arg<Indices>::Type>, \
        typename Traits::template Arg<Indices>::Type, \
        std::conditional_t<std::is_reference_v<typename Traits::template Arg<Indices>::Type>, \
            typename Traits::template Arg<Indices>::Type, \
            typename Traits::template Arg<Indices>::Type& \
        > \
    >>().value()...

    template <size_t... Indices> requires (Traits::IsStatic && Traits::HasReturn)
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        TRY(CheckArgs<Indices...>(args));

        return Variant::Create<typename Traits::ReturnType>(
            (ptr_)(REFLCPP_FUNCTION_WRAPPER_GET_ARGS())
        );
    }

    template <size_t... Indices> requires (Traits::IsStatic && !Traits::HasReturn)
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        TRY(CheckArgs<Indices...>(args));

        (ptr_)(
            REFLCPP_FUNCTION_WRAPPER_GET_ARGS()
        );

        return {};
    }

    template <size_t... Indices> requires (!Traits::IsStatic && !Traits::HasRReferenceObject && Traits::HasReturn)
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        TRY(CheckArgs<Indices...>(args));

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>;

        return Variant::Create<typename Traits::ReturnType>(
            (TRY(obj.Get<ClassT_>()).*ptr_)(REFLCPP_FUNCTION_WRAPPER_GET_ARGS())
        );
    }

    template <size_t... Indices> requires (!Traits::IsStatic && !Traits::HasRReferenceObject && !Traits::HasReturn)
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        TRY(CheckArgs<Indices...>(args));

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>;

        (TRY(obj.Get<ClassT_>()).*ptr_)(
            REFLCPP_FUNCTION_WRAPPER_GET_ARGS()
        );

        return {};
    }

    template <size_t... Indices> requires (!Traits::IsStatic && Traits::HasRReferenceObject && Traits::HasReturn)
    Result<Variant> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        TRY(CheckArgs<Indices...>(args));

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&&, typename Traits::ClassType&&>;

        return Variant::Create<typename Traits::ReturnType>(
            (std::move(TRY(obj.Get<ClassT_>())).*ptr_)(REFLCPP_FUNCTION_WRAPPER_GET_ARGS())
        );
    }

    template <size_t... Indices> requires (!Traits::IsStatic && Traits::HasRReferenceObject && !Traits::HasReturn)
    Result<void> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        TRY(CheckArgs<Indices...>(args));

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&&, typename Traits::ClassType&&>;

        (std::move(TRY(obj.Get<ClassT_>())).*ptr_)(
            REFLCPP_FUNCTION_WRAPPER_GET_ARGS()
        );

        return {};
    }

#undef REFLCPP_FUNCTION_WRAPPER_GET_ARGS

public:
    FunctionWrapper(T_ ptr)
        : ptr_(ptr) {}

    [[nodiscard]]
    std::vector<Result<TypeID>> GetArgTypes() const {
        return Traits::GetArgs();
    }

    [[nodiscard]]
    bool CanInvokeWithArgs(const ArgumentList& args) const {
        return !CheckArgs(args, std::make_index_sequence<Traits::ArgCount>()).HasError();
    }

    [[nodiscard]]
    Result<Variant> Invoke(const ArgumentList& args, const Variant& obj) const {
        if (args.size() != Traits::ArgCount) {
            return { RESULT_ERROR(), "incorrect number of arguments given {0} needed {1}", args.size(), Traits::ArgCount };
        }

        if constexpr (Traits::IsStatic) {
            if constexpr (Traits::HasReturn) {
                return InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>());
            }
            else {
                TRY(InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>()));
                return Variant::Void();
            }
        }
        else {
            if (obj.IsVoid()) {
                const auto& classType = TRY(Reflect<typename Traits::ClassType>());
                return { RESULT_ERROR(), "cannot invoke a member function without instance obj: {0}", classType.Dump() };
            }

            if constexpr (Traits::HasReturn) {
                return InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>(), obj);
            }
            else {
                TRY(InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>(), obj));
                return Variant::Void();
            }
        }
    }
};
}
