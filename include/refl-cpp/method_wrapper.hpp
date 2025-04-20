#pragma once

#include "refl-cpp/variant.hpp"
#include "refl-cpp/function_wrapper.hpp"
#include "refl-cpp/argument.hpp"

namespace ReflCpp {
struct MethodBase {
    virtual ~MethodBase() = default;

protected:
    [[nodiscard]]
    virtual Variant InvokeImpl(const ArgumentList& args, const Variant& instance) const = 0;

public:
    [[nodiscard]]
    virtual Variant InvokeStatic(const ArgumentList& args) const {
        return InvokeImpl(args, Variant::Void());
    }

    [[nodiscard]]
    virtual Variant Invoke(const Variant& instance, const ArgumentList& args) const {
        return InvokeImpl(args, instance);
    }
};

template <typename T>
struct MethodWrapper final : public MethodBase {
private:
    FunctionWrapper<T> func_;
    std::vector<const char*> argsNames_;

public:
    MethodWrapper(T func, const std::vector<const char*>& argsNames)
        : func_(func), argsNames_(argsNames) {}

    [[nodiscard]]
    bool CanInvokeWithArgs(const ArgumentList& args) const {
        return func_.CanInvokeWithArgs(args);
    }

    [[nodiscard]]
    Variant InvokeImpl(const ArgumentList& args, const Variant& instance) const override {
        return func_.Invoke(args, instance);
    }
};
}
