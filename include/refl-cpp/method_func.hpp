#pragma once

#include "refl-cpp/method_func_data.hpp"
#include "refl-cpp/function_wrapper.hpp"

namespace ReflCpp {
struct MethodFunc {
    virtual ~MethodFunc() = default;

    [[nodiscard]]
    virtual bool IsStatic() const = 0;

    [[nodiscard]]
    virtual Result<const std::vector<ArgumentInfo>&> GetArgs() const = 0;

    [[nodiscard]]
    virtual bool CanInvokeWithArgs(const ArgumentList& args) const = 0;

    [[nodiscard]]
    virtual Result<Variant> Invoke(const Variant& obj, const ArgumentList& args) const = 0;
};

template <typename T_>
struct MethodFuncWrapper final : MethodFunc {
private:
    using Traits = FunctionTraits<T_>;
    FunctionWrapper<T_> func_;
    std::vector<const char*> args_;

public:
    MethodFuncWrapper(const MethodFuncData<T_>& data)
        : func_(data.ptr), args_(data.args) {}

    [[nodiscard]]
    bool IsStatic() const override {
        return Traits::IsStatic;
    }

    //TODO: function to get argument information

    [[nodiscard]]
    Result<const std::vector<ArgumentInfo>&> GetArgs() const override {
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
    Result<Variant> Invoke(const Variant& obj, const ArgumentList& args) const override {
        return func_.Invoke(args, obj);
    }
};
}
