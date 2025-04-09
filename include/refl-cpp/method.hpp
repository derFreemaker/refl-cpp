#pragma once

#include <vector>

#include "refl-cpp/method_data.hpp"
#include "refl-cpp/variant.hpp"
#include "refl-cpp/argument.hpp"
#include "refl-cpp/method_wrapper.hpp"

namespace ReflCpp {
struct Method {
private:
    std::vector<MethodBase> funcs_;

    const char* name_;
    const std::vector<const char*> argsNames_;

public:
    template <typename... Funcs_>
    Method(const MethodData<Funcs_...>& builder)
        : funcs_({ MethodWrapper<Funcs_>(builder.ptr)... }),
          name_(builder.name),
          argsNames_(builder.arguments) {}

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
