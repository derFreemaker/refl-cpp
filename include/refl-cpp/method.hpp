#pragma once

#include <vector>

#include "refl-cpp/method_data.hpp"
#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"

namespace ReflCpp {
struct Method {
private:
    MethodBase& m_Func;

    const char* m_Name;
    const std::vector<ArgumentInfo> m_Arguments;

public:
    template <typename Func_>
    Method(const MethodData<Func_>& builder)
        : m_Func(*new MethodWrapper<Func_>(builder.func)),
          m_Name(builder.name),
          m_Arguments(builder.arguments) {}

    [[nodiscard]]
    const char* GetName() const {
        return m_Name;
    }

    [[nodiscard]]
    const Type& GetReturnType() const {
        return Reflect(m_Func.GetReturnType());
    }

    [[nodiscard]]
    const std::vector<ArgumentInfo>& GetArguments() const {
        return m_Arguments;
    }

    [[nodiscard]]
    const ArgumentInfo& GetArgument(const size_t index) const {
        return m_Arguments[index];
    }

    [[nodiscard]]
    bool IsStatic() const {
        return m_Func.GetIsStatic();
    }

    [[nodiscard]]
    bool IsConst() const {
        return m_Func.GetIsConst();
    }

    [[nodiscard]]
    Variant Invoke(const Variant& instance, const ArgumentList& args) const {
        return m_Func.Invoke(instance, args);
    }
};
}
