#pragma once

#include <vector>

#include "refl-cpp/variant.hpp"
#include "refl-cpp/function_wrapper.hpp"
#include "refl-cpp/argument.hpp"

namespace ReflCpp {
struct MethodBase {
    virtual ~MethodBase() = default;

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
    
    MethodWrapper(const Func_& func)
        : m_Func(func) {}

    [[nodiscard]]
    Variant InvokeStatic(const ArgumentList& arguments) const override {
        if constexpr (!m_Func.IsStatic()) {
            throw std::invalid_argument("not a static method");
        }
        else {
            return m_Func.Invoke(arguments);
        }
    }

    [[nodiscard]]
    Variant Invoke(const Variant& instance, const ArgumentList& arguments) const override {
        if constexpr (m_Func.IsStatic()) {
            return m_Func.Invoke(arguments);
        }
        else {
            return m_Func.Invoke(arguments, instance);
        }
    }
};
}
