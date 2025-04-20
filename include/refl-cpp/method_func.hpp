#pragma once

#include "refl-cpp/method_func_data.hpp"
#include "refl-cpp/function_wrapper.hpp"

namespace ReflCpp {
struct MethodFunc {
    virtual ~MethodFunc() = default;

    [[nodiscard]]
    virtual bool IsStatic() const = 0;

    [[nodiscard]]
    virtual const std::vector<ArgumentInfo>& GetArgs() const = 0;

    [[nodiscard]]
    virtual bool CanInvokeWithArgs(const ArgumentList& args) const = 0;

    [[nodiscard]]
    virtual Variant Invoke(const Variant& obj, const ArgumentList& args) const = 0;
};

template <typename T>
struct MethodFuncWrapper final : MethodFunc {
private:
    using Traits = FunctionTraits<T>;
    FunctionWrapper<T> func_;
    std::vector<const char*> args_;

public:
    MethodFuncWrapper(const MethodFuncData<T>& data)
        : func_(data.ptr), args_(data.args) {}

    [[nodiscard]]
    bool IsStatic() const override {
        return Traits::IsStatic;
    }

    //TODO: function to get argument information

    [[nodiscard]]
    const std::vector<ArgumentInfo>& GetArgs() const override {
        std::vector<ArgumentInfo> infoArgs{};
        if (!infoArgs.empty()) {
            return infoArgs;
        }

        auto types = func_.GetArgTypes();
        for (const auto& type : types) {
            if (type.HasError()) {
                return type.Error();
            }
        }

        infoArgs.reserve(types.size());
        for (uint8_t i = 0; i < types.size(); ++i) {
            infoArgs.emplace_back(args_[i], types[i].Value());
        }

        return infoArgs;
    }

    [[nodiscard]]
    bool CanInvokeWithArgs(const ArgumentList& args) const override {
        return func_.CanInvokeWithArgs(args);
    }

    [[nodiscard]]
    Variant Invoke(const Variant& obj, const ArgumentList& args) const override {
        return func_.Invoke(args, obj);
    }
};
}
