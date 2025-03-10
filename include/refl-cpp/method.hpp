#pragma once

#include <vector>

#include "refl-cpp/method_data.hpp"
#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"

namespace ReflCpp {
struct Method {
private:
    void* m_MethodPtr;

    const char* m_Name;
    const TypeID m_ReturnType = TypeID::Invalid;
    const std::vector<ArgumentInfo> m_Arguments;

    const bool m_IsStatic;
    const bool m_IsConst;

public:
    Method(const MethodData& data)
        : m_MethodPtr(data.method_ptr),

          m_Name(data.name),
          m_ReturnType(data.return_type),
          m_Arguments(data.arguments),

          m_IsStatic(data.is_static),
          m_IsConst(data.is_const) {}

    [[nodiscard]]
    const char* GetName() const {
        return m_Name;
    }

    [[nodiscard]]
    const Type& GetReturnType() const;

    [[nodiscard]]
    const std::vector<ArgumentInfo>& GetArguments() const {
        return m_Arguments;
    }

    [[nodiscard]]
    bool IsStatic() const {
        return m_IsStatic;
    }

    [[nodiscard]]
    bool IsConst() const {
        return m_IsConst;
    }

    template <typename ClassType, typename A1>
    Variant Invoke(Variant& instance, std::vector<Variant>& args) const {
        return (instance.GetValue<ClassType>().*m_MethodPtr)(
            args[0].GetValue<A1>()
        );
    }
};
}
