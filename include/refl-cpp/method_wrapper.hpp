#pragma once

#include <vector>

#include "refl-cpp/variant.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/function_traits.hpp"
#include "refl-cpp/function_wrapper.hpp"
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
    virtual bool HasLReferenceObject() const = 0;
    
    [[nodiscard]]
    virtual bool HasRReferenceObject() const = 0;
    
    [[nodiscard]]
    virtual TypeID GetReturnType() const = 0;

    [[nodiscard]]
    virtual std::vector<TypeID> GetArgumentsTypes() const = 0;

    [[nodiscard]]
    virtual int8_t GetArgumentsCount() const = 0;

    [[nodiscard]]
    virtual Variant InvokeStatic(const ArgumentList& arguments) const = 0;

    [[nodiscard]]
    virtual Variant Invoke(const Variant& instance, const ArgumentList& arguments) const = 0;
};

template <typename Func_>
struct MethodWrapper final : public MethodBase {
private:
    FunctionWrapper<Func_> m_Func;

public:
    using Traits = FunctionTraits<Func_>;

    MethodWrapper(const Func_& func)
        : m_Func(func) {}

    [[nodiscard]]
    constexpr bool IsStatic() const override {
        return m_Func.IsStatic();
    }

    [[nodiscard]]
    constexpr bool IsConst() const override {
        return m_Func.IsConst();
    }

    [[nodiscard]]
    constexpr bool HasLReferenceObject() const override {
        return m_Func.HasLReferenceObject();
    }

    [[nodiscard]]
    constexpr bool HasRReferenceObject() const override {
        return m_Func.HasRReferenceObject();
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
    Variant InvokeStatic(const ArgumentList& arguments) const override {
        if constexpr (!Traits::IsStatic) {
            throw std::invalid_argument("not a static method");
        }
        else {
            return m_Func.Invoke(arguments);
        }
    }

    [[nodiscard]]
    Variant Invoke(const Variant& instance, const ArgumentList& arguments) const override {
        if constexpr (Traits::IsStatic) {
            return m_Func.Invoke(arguments);
        }
        else {
            return m_Func.Invoke(arguments, instance);
        }
    }
};
}
