#pragma once

#include <array>

#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"
#include "refl-cpp/function_traits.hpp"

namespace ReflCpp {
template <typename T>
struct FunctionWrapper {
private:
    using Traits = FunctionTraits<T>;
    T ptr_;

    template <size_t... Indices>
    void CheckArgs(const ArgumentList& args) const {
        (args[Indices].template CheckGet<typename Traits::template Arg<Indices>::Type>() && ...);
    }

    template <size_t... Indices>
    void CheckArgs(const ArgumentList& args, std::index_sequence<Indices...>) const {
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
    >>()...

    template <size_t... Indices> requires (Traits::IsStatic && Traits::HasReturn)
    Variant InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        CheckArgs<Indices...>(args);

        return Variant::Create<typename Traits::ReturnType>(
            (ptr_)(REFLCPP_FUNCTION_WRAPPER_GET_ARGS())
        );
    }

    template <size_t... Indices> requires (Traits::IsStatic && !Traits::HasReturn)
    void InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        CheckArgs<Indices...>(args);

        (ptr_)(
            REFLCPP_FUNCTION_WRAPPER_GET_ARGS()
        );
    }

    template <size_t... Indices> requires (!Traits::IsStatic && !Traits::HasRReferenceObject && Traits::HasReturn)
    Variant InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        CheckArgs<Indices...>(args);

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>;

        return Variant::Create<typename Traits::ReturnType>(
            (obj.Get<ClassT_>().*ptr_)(REFLCPP_FUNCTION_WRAPPER_GET_ARGS())
        );
    }

    template <size_t... Indices> requires (!Traits::IsStatic && !Traits::HasRReferenceObject && !Traits::HasReturn)
    void InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        CheckArgs<Indices...>(args);

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&, typename Traits::ClassType&>;

        (obj.Get<ClassT_>().*ptr_)(
            REFLCPP_FUNCTION_WRAPPER_GET_ARGS()
        );
    }

    template <size_t... Indices> requires (!Traits::IsStatic && Traits::HasRReferenceObject && Traits::HasReturn)
    Variant InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        CheckArgs<Indices...>(args);

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&&, typename Traits::ClassType&&>;

        return Variant::Create<typename Traits::ReturnType>(
            (std::move(obj.Get<ClassT_>()).*ptr_)(REFLCPP_FUNCTION_WRAPPER_GET_ARGS())
        );
    }

    template <size_t... Indices> requires (!Traits::IsStatic && Traits::HasRReferenceObject && !Traits::HasReturn)
    void InvokeImpl(const ArgumentList& args, std::index_sequence<Indices...>, const Variant& obj) const {
        CheckArgs<Indices...>(args);

        using ClassT_ = std::conditional_t<Traits::IsConst, const typename Traits::ClassType&&, typename Traits::ClassType&&>;

        (std::move(obj.Get<ClassT_>()).*ptr_)(
            REFLCPP_FUNCTION_WRAPPER_GET_ARGS()
        );
    }

#undef REFLCPP_FUNCTION_WRAPPER_GET_ARGS

public:
    FunctionWrapper(T ptr)
        : ptr_(ptr) {}

    [[nodiscard]]
    std::vector<TypeID> GetArgTypes() const {
        return Traits::GetArgs();
    }

private:
    template <size_t... Indices>
    bool CanInvokeWithArgsImpl(const ArgumentList& args, std::index_sequence<Indices...>) const {
        return (args[Indices].template CanGet<typename Traits::template Arg<Indices>::Type>() && ...);
    }

public:
    [[nodiscard]]
    bool CanInvokeWithArgs(const ArgumentList& args) const {
        return CanInvokeWithArgsImpl(args, std::make_index_sequence<Traits::ArgCount>{});
    }

    [[nodiscard]]
    Variant Invoke(const ArgumentList& args, const Variant& obj) const {
        if (args.size() != Traits::ArgCount) {
            //TODO: better way for error handling
            throw std::logic_error(
                "incorrect number of arguments given " + std::to_string(args.size())
                + " needed " + std::to_string(Traits::ArgCount));
        }

        if constexpr (Traits::IsStatic) {
            if constexpr (Traits::HasReturn) {
                return InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>());
            }
            else {
                InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>());
                return Variant::Void();
            }
        }
        else {
            if (obj.IsVoid()) {
                const auto& classType = Reflect<typename Traits::ClassType>();
                throw std::logic_error("cannot invoke a member function without instance obj: " + classType.Dump());
            }

            if constexpr (Traits::HasReturn) {
                return InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>(), obj);
            }
            else {
                InvokeImpl(args, std::make_index_sequence<Traits::ArgCount>(), obj);
                return Variant::Void();
            }
        }
    }
};
}
