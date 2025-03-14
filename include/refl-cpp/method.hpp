#pragma once

#include <vector>

#include "refl-cpp/method_data.hpp"
#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"
#include "refl-cpp/method_wrapper.hpp"

namespace ReflCpp {
struct Method {
private:
    MethodBase* m_Func;

    const char* m_Name;
    const std::vector<const char*> m_ArgumentNames;

public:
    template <typename Func_>
    Method(const MethodData<Func_>& builder)
        : m_Func(new MethodWrapper<Func_>(builder.ptr)),
          m_Name(builder.name),
          m_ArgumentNames(builder.arguments) {}
    
    [[nodiscard]]
    const char* GetName() const {
        return m_Name;
    }

    [[nodiscard]]
    bool IsStatic() const {
        return m_Func->IsStatic();
    }

    [[nodiscard]]
    bool IsConst() const {
        return m_Func->IsConst();
    }

    [[nodiscard]]
    TypeID GetReturnType() const {
        return m_Func->GetReturnType();
    }

    [[nodiscard]]
    std::vector<ArgumentInfo> GetArguments() const {
        std::vector<ArgumentInfo> arguments;
        arguments.reserve(m_ArgumentNames.size());

        const auto& arg_types = m_Func->GetArgumentsTypes();
        for (int i = 0; i < m_ArgumentNames.size(); ++i) {
            arguments.emplace_back(m_ArgumentNames[i], arg_types[i]);
        }

        return arguments;
    }

    [[nodiscard]]
    ArgumentInfo GetArgument(const size_t index) const {
        if (index >= m_ArgumentNames.size()) {
            throw std::invalid_argument("index out of range");
        }
        
        return {m_ArgumentNames[index], m_Func->GetArgumentsTypes()[index]};
    }
    
    [[nodiscard]]
    Variant InvokeStatic(const ArgumentList& args) const {
        return m_Func->InvokeStatic(args);
    }

    [[nodiscard]]
    Variant Invoke(const Variant& instance) const {
        return m_Func->Invoke(instance, {});
    }
    
    [[nodiscard]]
    Variant Invoke(const Variant& instance, const ArgumentList& args) const {
        return m_Func->Invoke(instance, args);
    }

    void Print(std::ostream& stream) const;

    [[nodiscard]]
    std::string Dump() const;
};
}
