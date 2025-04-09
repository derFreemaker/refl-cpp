#pragma once

#include <vector>

#include "refl-cpp/method_data.hpp"
#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"
#include "refl-cpp/method_wrapper.hpp"

namespace ReflCpp {
struct Method {
private:
    const char* name_;
    std::vector<std::unique_ptr<MethodFunc>> funcs_;

public:
    Method(const MethodData& builder)
        : name_(builder.name),
          funcs_(builder.funcs) {}

    [[nodiscard]]
    const char* GetName() const {
        return name_;
    }

    // [[nodiscard]]
    // Result<Variant> InvokeStatic(const ArgumentList& args) const {
    //     return m_Funcs->InvokeStatic(args);
    // }

    // [[nodiscard]]
    // Result<Variant> Invoke(const Variant& instance, const ArgumentList& args = {}) const {
    //     return m_Funcs->Invoke(instance, args);
    // }

    // template <typename... Args>
    // [[nodiscard]]
    // Variant Invoke(const Variant& instance, Args&&... args) const {
    //     return m_Funcs->Invoke(instance, { Variant::Create<Args>(std::forward<Args>(args))... });
    // }
};
}
