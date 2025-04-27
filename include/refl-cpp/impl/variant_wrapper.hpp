#pragma once

#include "refl-cpp/variant.hpp"
#include "refl-cpp/common/unreachable.hpp"

namespace ReflCpp::detail {
struct VoidVariantWrapper final : public VariantWrapper<void> {
    void GetValue() override {}

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::VOID;
    }
};

template <typename T>
struct ValueVariantWrapper final : public VariantWrapper<T&> {
private:
    T value_;

public:
    ValueVariantWrapper(T& value) noexcept
        : value_(value) {}

    [[nodiscard]]
    T& GetValue() override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::VALUE;
    }
};

template <typename T>
struct ConstValueVariantWrapper final : public VariantWrapper<const T&> {
private:
    const T value_;

public:
    ConstValueVariantWrapper(const T& value) noexcept
        : value_(value) {}

    [[nodiscard]]
    const T& GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::CONST_VALUE;
    }
};

template <typename T>
struct LValueRefVariantWrapper final : public VariantWrapper<T&> {
private:
    T& value_;

public:
    LValueRefVariantWrapper(T& value) noexcept
        : value_(value) {}

    [[nodiscard]]
    T& GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::LVALUE_REF;
    }
};

template <typename T>
struct ConstLValueRefVariantWrapper final : public VariantWrapper<const T&> {
private:
    const T& value_;

public:
    ConstLValueRefVariantWrapper(const T& value) noexcept
        : value_(value) {}


    [[nodiscard]]
    const T& GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::CONST_LVALUE_REF;
    }
};

template <typename T>
struct RValueRefVariantWrapper final : public VariantWrapper<T&&> {
private:
    T&& value_;

public:
    RValueRefVariantWrapper(T&& value) noexcept
        : value_(std::move(value)) {}

    [[nodiscard]]
    T&& GetValue() noexcept override {
        return std::move(value_);
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::RVALUE_REF;
    }
};

template <typename T>
struct ConstRValueRefVariantWrapper final : public VariantWrapper<const T&&> {
private:
    const T&& value_;

public:
    ConstRValueRefVariantWrapper(const T&& value) noexcept
        : value_(std::move(value)) {}


    [[nodiscard]]
    const T&& GetValue() noexcept override {
        return std::move(value_);
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::CONST_RVALUE_REF;
    }
};

template <typename T>
struct PointerVariantWrapper final : public VariantWrapper<T*> {
private:
    T* value_;

public:
    PointerVariantWrapper(T* value) noexcept
        : value_(value) {}

    [[nodiscard]]
    T* GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::POINTER;
    }
};

template <typename T>
struct ConstPointerVariantWrapper final : public VariantWrapper<const T*> {
private:
    const T* value_;

public:
    ConstPointerVariantWrapper(const T* value) noexcept
        : value_(value) {}

    [[nodiscard]]
    const T* GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::CONST_POINTER;
    }
};

template <typename T, typename CleanT>
std::shared_ptr<VariantBase> MakeValueWrapper(T&& data) {
    if constexpr (std::is_copy_constructible_v<T>) {
        if constexpr (std::is_const_v<T>) {
            return std::make_shared<ConstValueVariantWrapper<CleanT>>(data);
        }
        else {
            return std::make_shared<ValueVariantWrapper<CleanT>>(data);
        }
    }
    else if constexpr (std::is_move_constructible_v<T>) {
        return std::make_shared<ValueVariantWrapper<CleanT>>(std::forward<T>(data));
    }
    else {
        return std::make_shared<ConstLValueRefVariantWrapper<CleanT>>(data);
    }
}

template <typename T, bool IsConst_ = false, typename CleanT_>
std::shared_ptr<VariantBase> MakeLValueRefWrapper(T&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstLValueRefVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<LValueRefVariantWrapper<CleanT_>>(data);
    }
}

template <typename T, bool IsConst_ = false, typename CleanT_>
std::shared_ptr<VariantBase> MakeRValueRefWrapper(T&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstRValueRefVariantWrapper<CleanT_>>(std::forward<T>(data));
    }
    else {
        return std::make_shared<RValueRefVariantWrapper<CleanT_>>(std::forward<T>(data));
    }
}

template <typename T, bool IsConst_ = false, typename CleanT_>
std::shared_ptr<VariantBase> MakePointerWrapper(T&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstPointerVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<PointerVariantWrapper<CleanT_>>(data);
    }
}

template <typename T>
std::shared_ptr<VariantBase> MakeWrapper(T&& data) {
    using CleanT = std::remove_const_t<std::remove_pointer_t<std::remove_reference_t<T>>>;

    if constexpr (std::is_lvalue_reference_v<T>) {
        return MakeLValueRefWrapper<T, std::is_const_v<std::remove_reference_t<T>>, CleanT>(std::forward<T>(data));
    }
    else if constexpr (std::is_rvalue_reference_v<T>) {
        return MakeRValueRefWrapper<T, std::is_const_v<std::remove_reference_t<T>>, CleanT>(std::forward<T>(data));
    }
    else if constexpr (std::is_pointer_v<T>) {
        return MakePointerWrapper<T, std::is_const_v<std::remove_pointer_t<T>>, CleanT>(std::forward<T>(data));
    }
    else {
        if constexpr (std::is_copy_constructible_v<CleanT> || std::is_move_constructible_v<CleanT>) {
            // we don't care about const since we copy the value anyway
            return MakeValueWrapper<T, CleanT>(std::forward<T>(data));
        }
        else {
            return MakeLValueRefWrapper<T, std::is_const_v<T>, CleanT>(std::forward<T>(data));
        }
    }

    unreachable<true>();
}
}
