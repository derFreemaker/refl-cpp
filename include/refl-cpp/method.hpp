#pragma once

#include <vector>

#include "refl-cpp/method_data.hpp"
#include "refl-cpp/method_wrapper.hpp"

namespace ReflCpp {
struct Method {
private:
    const char* name_;
    std::vector<std::shared_ptr<MethodFunc>> funcs_;

public:
    Method(const MethodData& builder)
        : name_(builder.name),
          funcs_(builder.funcs) {}

    [[nodiscard]]
    const char* GetName() const {
        return name_;
    }

    [[nodiscard]]
    const std::vector<std::shared_ptr<MethodFunc>>& GetFunctions() const {
        return funcs_;
    }

    [[nodiscard]]
    std::shared_ptr<MethodFunc> GetFunction(const size_t index) const {
        return funcs_[index];
    }

    [[nodiscard]]
    Result<Variant> Invoke(const ArgumentList& args) const {
        for (const auto& func : funcs_) {
            if (!func->IsStatic() && !func->CanInvokeWithArgs(args)) {
                continue;
            }
            return func->Invoke(Variant::Void(), args);
        }

        //TODO: better error give feedback about passed types
        return { RESULT_ERROR(), "no matching static function found" };
    }

    [[nodiscard]]
    Result<Variant> Invoke(const Variant& obj, const ArgumentList& args) const {
        for (const auto& func : funcs_) {
            if (!func->CanInvokeWithArgs(args)) {
                continue;
            }
            return func->Invoke(obj, args);
        }

        //TODO: better error give feedback about passed types
        return { RESULT_ERROR(), "no matching function found" };
    }
};
}
