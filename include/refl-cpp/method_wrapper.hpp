#pragma once

#include <optional>
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
    virtual bool GetIsStatic() const = 0;

    [[nodiscard]]
    virtual bool GetIsConst() const = 0;

    [[nodiscard]]
    virtual TypeID GetReturnType() const = 0;

    [[nodiscard]]
    virtual std::vector<TypeID> GetArgumentsTypes() const = 0;

    [[nodiscard]]
    virtual int8_t GetArgumentsCount() const = 0;

    [[nodiscard]]
    virtual Variant Invoke(const Variant& instance, const ArgumentList& arguments) const = 0;
};

namespace detail {
template <typename Func_, size_t... Indices>
Variant InvokeImpl(const Variant& instance, Func_ func, const ArgumentList& arguments, std::index_sequence<Indices...>) {
    if constexpr (std::is_void_v<typename FunctionTraits<Func_>::ReturnType>) {
        (instance.GetValue<typename FunctionTraits<Func_>::ClassType>()->*func)(
            static_cast<typename FunctionTraits<Func_>::template Arg<Indices>::Type>(arguments[Indices].GetDataRaw())...
        );

        return Variant(nullptr, ReflectID<void>());
    }
    else {
        return Variant(
            (instance.GetValue<typename FunctionTraits<Func_>::ClassType>()->*func)(
                static_cast<typename FunctionTraits<Func_>::template Arg<Indices>::Type>(arguments[Indices].GetDataRaw())...
            )
        );
    }
}
}

template <typename Func_>
struct MethodWrapper final : public MethodBase {
private:
    Func_ m_Func;
    using m_FuncTraits = FunctionTraits<Func_>;

public:
    MethodWrapper(const Func_& func)
        : m_Func(func) {}

    [[nodiscard]]
    constexpr bool GetIsStatic() const override {
        return m_FuncTraits::IsStatic;
    }

    [[nodiscard]]
    constexpr bool GetIsConst() const override {
        return m_FuncTraits::IsConst;
    }

    [[nodiscard]]
    TypeID GetReturnType() const override {
        return ReflectID<typename m_FuncTraits::ReturnType>();
    }

    [[nodiscard]]
    std::vector<TypeID> GetArgumentsTypes() const override {
        return detail::ReflectTupleTypes<typename m_FuncTraits::Args>();
    }

    [[nodiscard]]
    constexpr int8_t GetArgumentsCount() const override {
        return m_FuncTraits::ArgCount;
    }

    [[nodiscard]]
    Variant Invoke(const Variant& instance, const ArgumentList& arguments) const override {
        if (arguments.size() != m_FuncTraits::ArgCount) {
            throw std::invalid_argument("incorrect number of arguments");
        }

        return detail::InvokeImpl(instance, m_Func, arguments, std::make_index_sequence<m_FuncTraits::ArgCount>{});
    }
};
};
