#pragma once

#include <vector>

#include "refl-cpp/variant.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/function_traits.hpp"
#include "refl-cpp/argument.hpp"
#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct MethodBase {
    virtual ~MethodBase() = default;

    [[nodiscard]]
    virtual bool IsStatic() const = 0;

    [[nodiscard]]
    virtual bool IsConst() const = 0;

    [[nodiscard]]
    virtual TypeID GetReturnType() const = 0;

    [[nodiscard]]
    virtual std::vector<TypeID> GetArgumentsTypes() const = 0;

    [[nodiscard]]
    virtual int8_t GetArgumentsCount() const = 0;

    [[nodiscard]]
    virtual Variant Invoke(const ArgumentList& arguments) const = 0;

    [[nodiscard]]
    virtual Variant Invoke(const Variant& instance, const ArgumentList& arguments) const = 0;
};

namespace detail {
template <typename Func_, size_t... Indices>
Variant InvokeFunctionImpl(Func_ func, const ArgumentList& args, std::index_sequence<Indices...>) {
    if (args.Size() != FunctionTraits<Func_>::ArgCount) {
        throw std::invalid_argument("incorrect number of arguments");
    }

    if constexpr (std::is_void_v<typename FunctionTraits<Func_>::ReturnType>) {
        func(
            args[Indices].GetValue < typename FunctionTraits<Func_>::template Arg<Indices>::Type > ()...
        );

        return Variant::Void();
    }
    else {
        return Variant(
            func(
                args[Indices].GetValue < typename FunctionTraits<Func_>::template Arg<Indices>::Type > ()...
            )
        );
    }
}

template <typename Func_>
Variant InvokeFunction(Func_ func, const ArgumentList& arguments) {
    return InvokeFunctionImpl(func, arguments, std::make_index_sequence<FunctionTraits<Func_>::ArgCount>());
}
}

template <typename Func_>
struct MethodWrapper final : public MethodBase {
private:
    Func_ m_Func;
    using Traits = FunctionTraits<Func_>;

public:
    MethodWrapper(const Func_& func)
        : m_Func(func) {}

    [[nodiscard]]
    constexpr bool IsStatic() const override {
        return Traits::IsStatic;
    }

    [[nodiscard]]
    constexpr bool IsConst() const override {
        return Traits::IsConst;
    }

    [[nodiscard]]
    TypeID GetReturnType() const override {
        return ReflectID<typename Traits::ReturnType>();
    }

    [[nodiscard]]
    std::vector<TypeID> GetArgumentsTypes() const override {
        return detail::ReflectTupleTypes<typename Traits::Args>();
    }

    [[nodiscard]]
    constexpr int8_t GetArgumentsCount() const override {
        return Traits::ArgCount;
    }

    [[nodiscard]]
    Variant Invoke(const ArgumentList& arguments) const override {
        if constexpr (!Traits::IsStatic) {
            throw std::invalid_argument("not a static method");
        }
        else {
            return detail::InvokeFunction(m_Func, arguments);
        }
    }

    [[nodiscard]]
    Variant Invoke(const Variant& instance, const ArgumentList& arguments) const override {
        if constexpr (Traits::IsStatic) {
            return detail::InvokeFunction(m_Func, arguments);
        }
        else {
            auto func = instance.GetValue<typename Traits::ClassType>().*m_Func;
            return detail::InvokeFunction(func, arguments, std::make_index_sequence<Traits::ArgCount>{});
        }
    }
};
}
