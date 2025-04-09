#pragma once

#include "refl-cpp/variant.hpp"
#include "refl-cpp/function_wrapper.hpp"
#include "refl-cpp/argument.hpp"

namespace ReflCpp {
struct MethodBase {
    virtual ~MethodBase() = default;

protected:
    [[nodiscard]]
    virtual Result<Variant> InvokeImpl(const ArgumentList& args, const Variant& instance) const = 0;

public:
    [[nodiscard]]
    virtual Result<Variant> InvokeStatic(const ArgumentList& args) const {
        return InvokeImpl(args, Variant::Void());
    }

    [[nodiscard]]
    virtual Result<Variant> Invoke(const Variant& instance, const ArgumentList& args) const {
        return InvokeImpl(args, instance);
    }
};

template <typename T_>
struct MethodWrapper final : public MethodBase {
private:
    FunctionWrapper<T_> func_;
    std::vector<const char*> argsNames_;

public:
    MethodWrapper(T_ func, const std::vector<const char*>& argsNames)
        : func_(func), argsNames_(argsNames) {}

    [[nodiscard]]
    bool CanInvokeWithArgs(const ArgumentList& args) const {
        return func_.CanInvokeWithArgs(args);
    }

    [[nodiscard]]
    Result<Variant> InvokeImpl(const ArgumentList& args, const Variant& instance) const override {
        return func_.Invoke(args, instance);
    }
};
}
