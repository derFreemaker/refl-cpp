#pragma once

#include "refl-cpp/method_func_data.hpp"
#include "refl-cpp/function_wrapper.hpp"

namespace ReflCpp {
struct MethodFunc {
    virtual ~MethodFunc() = default;

    [[nodiscard]]
    virtual Result<std::vector<TypeID>> GetArgTypes() const = 0;

    [[nodiscard]]
    virtual bool CanInvokeWithArgs(const ArgumentList& args) const = 0;

    [[nodiscard]]
    virtual Result<Variant> Invoke(const Variant& obj, const ArgumentList& args) const = 0;
};

template <typename T_>
struct MethodFuncWrapper final : MethodFunc {
private:
    FunctionWrapper<T_> func_;
    std::vector<const char*> argNames_;

public:
    MethodFuncWrapper(const MethodFuncData<T_>& data)
        : func_(data.ptr), argNames_(data.argNames) {}

    [[nodiscard]]
    Result<std::vector<TypeID>> GetArgTypes() const override {
        return func_.GetArgTypes();
    }

    [[nodiscard]]
    bool CanInvokeWithArgs(const ArgumentList& args) const override {
        return func_.CanInvokeWithArgs(args);
    }

    [[nodiscard]]
    Result<Variant> Invoke(const Variant& obj, const ArgumentList& args) const override {
        return func_.Invoke(args, obj);
    }
};
}
