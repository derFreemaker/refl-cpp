#pragma once

#include <array>

#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"
#include "refl-cpp/function_traits.hpp"

namespace ReflCpp {
struct FunctionWrapperArgumentError {
    VariantGetError error;
    size_t index;

    inline constexpr FunctionWrapperArgumentError(const VariantGetError& error, const size_t index) noexcept
        : error(error), index(index) {}
};

struct FunctionWrapperInvokeError {
    enum class Type : uint8_t {
        IncorrectNumberOfArguments = 0,
        ObjectIsVoid,
        ObjectIsInvalid,
        ArgumentIsInvalid,

        MaxReflectLimitReached,
        ReflectTypeCreationFailed,
        OutOfMemory,

        NoCompatibleFunctionFound,
    };

    Type type;
    std::optional<FunctionWrapperArgumentError> arg_error;

    FunctionWrapperInvokeError() = delete;

    inline constexpr FunctionWrapperInvokeError(const Type type) noexcept
        : type(type) {}

    inline constexpr FunctionWrapperInvokeError(const FunctionWrapperArgumentError& arg_error) noexcept
        : type(Type::ArgumentIsInvalid), arg_error(arg_error) {}

    inline constexpr FunctionWrapperInvokeError(const ReflectError reflect_error) noexcept {
        switch (reflect_error) {
            case ReflectError::MaxLimitReached:
                type = Type::MaxReflectLimitReached;
                break;
            case ReflectError::CreationFailed:
                type = Type::ReflectTypeCreationFailed;
                break;
            case ReflectError::OutOfMemory:
                type = Type::OutOfMemory;
                break;
        }
    }
};

template <typename T>
struct FunctionWrapper {
private:
    using Traits = FunctionTraits<T>;
    T ptr_;

    template <size_t First, size_t... Rest>
    [[nodiscard]]
    rescpp::result<void, FunctionWrapperArgumentError> CheckArgsImpl(const ArgumentList& args) const noexcept {
        RESCPP_TRY_IMPL(args[First].CheckGet<typename Traits::template Arg<First>::Type>(), {
                        return rescpp::fail<FunctionWrapperArgumentError>(result_.error(), First);
                        });

        if constexpr (sizeof...(Rest) == 0) {
            return {};
        }
        else {
            return CheckArgsImpl<Rest...>(args);
        }
    }

    template <size_t... Indices>
    [[nodiscard]]
    rescpp::result<void, FunctionWrapperArgumentError> CheckArgs(const ArgumentList& args) const {
        if constexpr (sizeof...(Indices) == 0) {
            return {};
        }
        else {
            return CheckArgsImpl<Indices...>(args);
        }
    }

    template <size_t... Indices>
    [[nodiscard]]
    rescpp::result<void, FunctionWrapperArgumentError> CheckArgs(const ArgumentList& args, std::index_sequence<Indices...>) const {
        return CheckArgs<Indices...>(args);
    }

    [[nodiscard]]
    rescpp::result<void, FunctionWrapperArgumentError> CheckArgs(const ArgumentList& args) const noexcept {
        return CheckArgs(args, std::make_index_sequence<Traits::ArgCount>{});
    }

#define REFLCPP_FUNCTION_WRAPPER_CHECK_ARGS() \
    TRY(CheckArgs<Indices...>(args))

#define REFLCPP_FUNCTION_WRAPPER_GET_ARGS() \
    args[Indices].template Get<std::conditional_t< \
        std::is_pointer_v<typename Traits::template Arg<Indices>::Type>, \
        typename Traits::template Arg<Indices>::Type, \
        std::conditional_t<std::is_reference_v<typename Traits::template Arg<Indices>::Type>, \
            typename Traits::template Arg<Indices>::Type, \
            typename Traits::template Arg<Indices>::Type& \
        > \
    >>().value()...

    template <size_t... Indices>
        requires (Traits::IsStatic && Traits::HasReturn)
    rescpp::result<Variant, FunctionWrapperInvokeError> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        REFLCPP_FUNCTION_WRAPPER_CHECK_ARGS();

        return Variant::Create<typename Traits::ReturnType>(
            (ptr_)(REFLCPP_FUNCTION_WRAPPER_GET_ARGS())
        );
    }

    template <size_t... Indices>
        requires (Traits::IsStatic && !Traits::HasReturn)
    rescpp::result<void, FunctionWrapperInvokeError> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        REFLCPP_FUNCTION_WRAPPER_CHECK_ARGS();
        
        (ptr_)(
            REFLCPP_FUNCTION_WRAPPER_GET_ARGS()
        );

        return {};
    }

    template <size_t... Indices>
        requires (!Traits::IsStatic && !Traits::HasRReferenceObject && Traits::HasReturn)
    rescpp::result<Variant, FunctionWrapperInvokeError> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        REFLCPP_FUNCTION_WRAPPER_CHECK_ARGS();
        
        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>;
        if (!obj.CanGet<ClassT_>()) {
            return rescpp::fail(FunctionWrapperInvokeError::Type::ObjectIsInvalid);
        }

        return Variant::Create<typename Traits::ReturnType>(
            (obj.Get<ClassT_>().value().*ptr_)(REFLCPP_FUNCTION_WRAPPER_GET_ARGS())
        );
    }

    template <size_t... Indices>
        requires (!Traits::IsStatic && !Traits::HasRReferenceObject && !Traits::HasReturn)
    rescpp::result<void, FunctionWrapperInvokeError> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        REFLCPP_FUNCTION_WRAPPER_CHECK_ARGS();
        
        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>;
        if (!obj.CanGet<ClassT_>()) {
            return rescpp::fail(FunctionWrapperInvokeError::Type::ObjectIsInvalid);
        }

        (obj.Get<ClassT_>().value().*ptr_)(
            REFLCPP_FUNCTION_WRAPPER_GET_ARGS()
        );

        return {};
    }

    template <size_t... Indices>
        requires (!Traits::IsStatic && Traits::HasRReferenceObject && Traits::HasReturn)
    rescpp::result<Variant, FunctionWrapperInvokeError> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        REFLCPP_FUNCTION_WRAPPER_CHECK_ARGS();
        
        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&&, typename Traits::ClassType&&>;
        if (!obj.CanGet<ClassT_>()) {
            return rescpp::fail(FunctionWrapperInvokeError::Type::ObjectIsInvalid);
        }

        return Variant::Create<typename Traits::ReturnType>(
            (std::move(obj.Get<ClassT_>().value()).*ptr_)(REFLCPP_FUNCTION_WRAPPER_GET_ARGS())
        );
    }

    template <size_t... Indices>
        requires (!Traits::IsStatic && Traits::HasRReferenceObject && !Traits::HasReturn)
    rescpp::result<void, FunctionWrapperInvokeError> InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        REFLCPP_FUNCTION_WRAPPER_CHECK_ARGS();
        
        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&&, typename Traits::ClassType&&>;
        if (!obj.CanGet<ClassT_>()) {
            return rescpp::fail(FunctionWrapperInvokeError::Type::ObjectIsInvalid);
        }

        (std::move(obj.Get<ClassT_>().value()).*ptr_)(
            REFLCPP_FUNCTION_WRAPPER_GET_ARGS()
        );

        return {};
    }

#undef REFLCPP_FUNCTION_WRAPPER_GET_ARGS
#undef REFLCPP_FUNCTION_WRAPPER_CHECK_ARGS

public:
    FunctionWrapper(T ptr)
        : ptr_(ptr) {}

    [[nodiscard]]
    rescpp::result<const std::vector<TypeID>&, ReflectError> GetArgTypes() const {
        return Traits::GetArgs();
    }

public:
    [[nodiscard]]
    bool CanInvokeWithArgs(const ArgumentList& args) const {
        return !CheckArgs(args).has_error();
    }

    [[nodiscard]]
    rescpp::result<Variant, FunctionWrapperInvokeError> Invoke(const ArgumentList& args, const Variant& obj) const {
        if (args.size() != Traits::ArgCount) {
            //TODO: better way for error handling
            return rescpp::fail(FunctionWrapperInvokeError::Type::IncorrectNumberOfArguments);
        }

        if constexpr (Traits::IsStatic) {
            if constexpr (Traits::HasReturn) {
                return TRY(InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>()));
            }
            else {
                TRY(InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>()));
                return Variant::Void();
            }
        }
        else {
            if (obj.IsVoid()) {
                return rescpp::fail(FunctionWrapperInvokeError::Type::ObjectIsVoid);
            }

            if constexpr (Traits::HasReturn) {
                return TRY(InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>(), obj));
            }
            else {
                TRY(InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>(), obj));
                return Variant::Void();
            }
        }
    }
};
}
