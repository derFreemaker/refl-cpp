#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp::detail {
struct VoidVariantWrapper final : public VariantWrapper<void> {
    void GetValue() override {}

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::VOID;
    }
};

template <typename T_>
struct ValueVariantWrapper final : public VariantWrapper<T_&> {
private:
    T_ value_;

public:
    ValueVariantWrapper(T_& value) noexcept
        : value_(value) {}

    [[nodiscard]]
    T_& GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::VALUE;
    }
};

template <typename T_>
struct ConstValueVariantWrapper final : public VariantWrapper<const T_&> {
private:
    const T_ value_;

public:
    ConstValueVariantWrapper(const T_& value) noexcept
        : value_(value) {}

    [[nodiscard]]
    const T_& GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::CONST_VALUE;
    }
};

template <typename T_>
struct LValueRefVariantWrapper final : public VariantWrapper<T_&> {
private:
    T_& value_;

public:
    LValueRefVariantWrapper(T_& value) noexcept
        : value_(value) {}

    [[nodiscard]]
    T_& GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::LVALUE_REF;
    }
};

template <typename T_>
struct ConstLValueRefVariantWrapper final : public VariantWrapper<const T_&> {
private:
    const T_& value_;

public:
    ConstLValueRefVariantWrapper(const T_& value) noexcept
        : value_(value) {}


    [[nodiscard]]
    const T_& GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::CONST_LVALUE_REF;
    }
};

template <typename T_>
struct RValueRefVariantWrapper final : public VariantWrapper<T_&&> {
private:
    T_&& value_;

public:
    RValueRefVariantWrapper(T_&& value) noexcept
        : value_(std::move(value)) {}

    [[nodiscard]]
    T_&& GetValue() noexcept override {
        return std::move(value_);
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::RVALUE_REF;
    }
};

template <typename T_>
struct ConstRValueRefVariantWrapper final : public VariantWrapper<const T_&&> {
private:
    const T_&& value_;

public:
    ConstRValueRefVariantWrapper(const T_&& value) noexcept
        : value_(std::move(value)) {}


    [[nodiscard]]
    const T_&& GetValue() noexcept override {
        return std::move(value_);
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::CONST_RVALUE_REF;
    }
};

template <typename T_>
struct PointerVariantWrapper final : public VariantWrapper<T_*> {
private:
    T_* value_;

public:
    PointerVariantWrapper(T_* value) noexcept
        : value_(value) {}

    [[nodiscard]]
    T_* GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::POINTER;
    }
};

template <typename T_>
struct ConstPointerVariantWrapper final : public VariantWrapper<const T_*> {
private:
    const T_* value_;

public:
    ConstPointerVariantWrapper(const T_* value) noexcept
        : value_(value) {}

    [[nodiscard]]
    const T_* GetValue() noexcept override {
        return value_;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const noexcept override {
        return VariantWrapperType::CONST_POINTER;
    }
};

template <typename T_, typename CleanT>
std::shared_ptr<VariantBase> MakeValueWrapper(T_&& data) noexcept {
    if constexpr (std::is_copy_constructible_v<T_>) {
        if constexpr (std::is_const_v<T_>) {
            return std::make_shared<ConstValueVariantWrapper<CleanT>>(data);
        }
        else {
            return std::make_shared<ValueVariantWrapper<CleanT>>(data);
        }
    }
    else if constexpr (std::is_move_constructible_v<T_>) {
        return std::make_shared<ValueVariantWrapper<CleanT>>(std::forward<T_>(data));
    }
    else {
        return std::make_shared<ConstLValueRefVariantWrapper<CleanT>>(data);
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
std::shared_ptr<VariantBase> MakeLValueRefWrapper(T_&& data) noexcept {
    if constexpr (IsConst_) {
        return std::make_shared<ConstLValueRefVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<LValueRefVariantWrapper<CleanT_>>(data);
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
std::shared_ptr<VariantBase> MakeRValueRefWrapper(T_&& data) noexcept {
    if constexpr (IsConst_) {
        return std::make_shared<ConstRValueRefVariantWrapper<CleanT_>>(std::forward<T_>(data));
    }
    else {
        return std::make_shared<RValueRefVariantWrapper<CleanT_>>(std::forward<T_>(data));
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
std::shared_ptr<VariantBase> MakePointerWrapper(T_&& data) noexcept {
    if constexpr (IsConst_) {
        return std::make_shared<ConstPointerVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<PointerVariantWrapper<CleanT_>>(data);
    }
}

template <typename T_>
std::shared_ptr<VariantBase> MakeWrapper(T_&& data) noexcept {
    using CleanT = std::remove_const_t<std::remove_pointer_t<std::remove_reference_t<T_>>>;

    if constexpr (std::is_lvalue_reference_v<T_>) {
        return MakeLValueRefWrapper<T_, std::is_const_v<std::remove_reference_t<T_>>, CleanT>(std::forward<T_>(data));
    }
    else if constexpr (std::is_rvalue_reference_v<T_>) {
        return MakeRValueRefWrapper<T_, std::is_const_v<std::remove_reference_t<T_>>, CleanT>(std::forward<T_>(data));
    }
    else if constexpr (std::is_pointer_v<T_>) {
        return MakePointerWrapper<T_, std::is_const_v<std::remove_pointer_t<T_>>, CleanT>(std::forward<T_>(data));
    }
    else {
        if constexpr (std::is_copy_constructible_v<CleanT> || std::is_move_constructible_v<CleanT>) {
            // we don't care about const since we copy the value anyway
            return MakeValueWrapper<T_, CleanT>(std::forward<T_>(data));
        }
        else {
            return MakeLValueRefWrapper<T_, std::is_const_v<T_>, CleanT>(std::forward<T_>(data));
        }
    }

    static_assert("should be unreachable");
}
}
