#pragma once

#include "refl-cpp/method_func_data.hpp"
#include "refl-cpp/function_wrapper.hpp"

namespace ReflCpp {
struct MethodFunc {
    virtual ~MethodFunc() = default;

    [[nodiscard]]
    virtual bool IsStatic() const = 0;

    [[nodiscard]]
    virtual rescpp::result<const std::vector<ArgumentInfo>&, ReflectError> GetArgs() const = 0;

    [[nodiscard]]
    virtual bool CanInvokeWithArgs(const ArgumentList& args) const = 0;

    [[nodiscard]]
    virtual rescpp::result<Variant, FunctionWrapperInvokeError> Invoke(const Variant& obj, const ArgumentList& args) const = 0;
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
    rescpp::result<const std::vector<ArgumentInfo>&, ReflectError> GetArgs() const override {
        static std::vector<ArgumentInfo> infoArgs{};
        if (!infoArgs.empty()) {
            return infoArgs;
        }

        auto types = TRY(func_.GetArgTypes());

        infoArgs.reserve(types.size());
        for (uint8_t i = 0; i < types.size(); ++i) {
            infoArgs.emplace_back(args_[i], types[i]);
        }

        return infoArgs;
    }

    [[nodiscard]]
    bool CanInvokeWithArgs(const ArgumentList& args) const override {
        return func_.CanInvokeWithArgs(args);
    }

    [[nodiscard]]
    rescpp::result<Variant, FunctionWrapperInvokeError> Invoke(const Variant& obj, const ArgumentList& args) const override {
        return func_.Invoke(args, obj);
    }
};
}
