#pragma once

#include "refl-cpp/variant.hpp"

#include "refl-cpp/reflect.hpp"

namespace ReflCpp {

namespace detail {
struct VoidVariantWrapper final : public VariantWrapper<void> {
    void GetValue() override {}

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::VOID;
    }
};

template <typename T_>
struct ValueVariantWrapper final : public VariantWrapper<T_&> {
private:
    T_ m_Value;

public:
    ValueVariantWrapper(T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    T_& GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::VALUE;
    }
};

template <typename T_>
struct ConstValueVariantWrapper final : public VariantWrapper<const T_&> {
private:
    const T_ m_Value;

public:
    ConstValueVariantWrapper(const T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::CONST_VALUE;
    }
};

template <typename T_>
struct LValueRefVariantWrapper final : public VariantWrapper<T_&> {
private:
    T_& m_Value;

public:
    LValueRefVariantWrapper(T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    T_& GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::LVALUE_REF;
    }
};

template <typename T_>
struct ConstLValueRefVariantWrapper final : public VariantWrapper<const T_&> {
private:
    const T_& m_Value;

public:
    ConstLValueRefVariantWrapper(const T_& value)
        : m_Value(value) {}


    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::CONST_LVALUE_REF;
    }
};

template <typename T_>
struct RValueRefVariantWrapper final : public VariantWrapper<T_&&> {
private:
    T_&& m_Value;

public:
    RValueRefVariantWrapper(T_&& value)
        : m_Value(std::move(value)) {}

    [[nodiscard]]
    T_&& GetValue() override {
        return std::move(m_Value);
    }

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::RVALUE_REF;
    }
};

template <typename T_>
struct ConstRValueRefVariantWrapper final : public VariantWrapper<const T_&&> {
private:
    const T_&& m_Value;

public:
    ConstRValueRefVariantWrapper(const T_&& value)
        : m_Value(std::move(value)) {}


    [[nodiscard]]
    const T_&& GetValue() override {
        return std::move(m_Value);
    }

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::CONST_RVALUE_REF;
    }
};

template <typename T_>
struct PointerVariantWrapper final : public VariantWrapper<T_*> {
private:
    T_* m_Value;

public:
    PointerVariantWrapper(T_* value)
        : m_Value(value) {}

    [[nodiscard]]
    T_* GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::POINTER;
    }
};

template <typename T_>
struct ConstPointerVariantWrapper final : public VariantWrapper<const T_*> {
private:
    const T_* m_Value;

public:
    ConstPointerVariantWrapper(const T_* value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_* GetValue() override {
        return m_Value;
    }

    [[nodiscard]]
    VariantWrapperType GetType() const override {
        return VariantWrapperType::CONST_POINTER;
    }
};

template <typename T_, typename CleanT>
std::shared_ptr<VariantBase> MakeValueWrapper(T_&& data) {
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
std::shared_ptr<VariantBase> MakeLValueRefWrapper(T_&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstLValueRefVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<LValueRefVariantWrapper<CleanT_>>(data);
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
std::shared_ptr<VariantBase> MakeRValueRefWrapper(T_&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstRValueRefVariantWrapper<CleanT_>>(std::forward<T_>(data));
    }
    else {
        return std::make_shared<RValueRefVariantWrapper<CleanT_>>(std::forward<T_>(data));
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
std::shared_ptr<VariantBase> MakePointerWrapper(T_&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstPointerVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<PointerVariantWrapper<CleanT_>>(data);
    }
}

template <typename T_>
std::shared_ptr<VariantBase> MakeWrapper(T_&& data) {
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

template <typename R_, VariantWrapperType Type_>
struct VariantMatcher {
    static bool Match(const TypeID _) {
        // no implementation
        return false;
    }
};

template <typename R_>
struct VariantMatcher<R_, VariantWrapperType::VOID> {
    static bool Match(const TypeID _) {
        return false;
    }
};

template <typename R_>
    requires (!std::is_pointer_v<R_> && !std::is_reference_v<R_>)
struct VariantMatcher<R_, VariantWrapperType::VALUE> {
    static bool Match(const TypeID type) {
        return type.Equals<std::remove_const_t<R_>>();
    }
    static Result<std::remove_const_t<R_>&> Get(const VariantBase* base) {
        return static_cast<VariantWrapper<std::remove_const_t<R_>&>*>(base)->GetValue();
    }
};
}

template <typename T_>
Variant Variant::Create(T_&& data) {
    constexpr bool isConst = std::is_const_v<T_>;
    return Variant(detail::MakeWrapper<T_>(std::forward<T_>(data)), ReflectID<T_>().Value(), isConst);
}

inline FormattedError Variant::CanNotGetFromVariantWithType(const Type& type, const Type& passed_type) {
    return FormattedError {
        "cannot get from Variant ({0}) with passed type: {1}",
        type.Dump(),
        passed_type.Dump()
    };
}

inline Variant& Variant::Void() {
    static auto instance = Variant(
        std::make_shared<detail::VoidVariantWrapper>(),
        ReflectID<void>().Value(),
        false
    );
    return instance;
}

template <typename T_>
bool Variant::CanGet() const {
    using detail::VariantWrapperType;

    const VariantWrapperType wrapperType = m_Base->GetType();

    constexpr bool is_const_requested = std::is_const_v<std::remove_reference_t<std::remove_pointer_t<T_>>>;

    if (!is_const_requested &&
        (wrapperType == VariantWrapperType::CONST_VALUE
            || wrapperType == VariantWrapperType::CONST_LVALUE_REF
            || wrapperType == VariantWrapperType::CONST_RVALUE_REF
            || wrapperType == VariantWrapperType::CONST_POINTER)) {
        return false;
    }

    if constexpr (std::is_lvalue_reference_v<T_>) {
        if (wrapperType == VariantWrapperType::VALUE) {
            return m_Type.Equals<std::remove_const_t<std::remove_reference_t<T_>>>();
        }

        if (wrapperType == VariantWrapperType::CONST_VALUE) {
            return m_Type.Equals<std::remove_reference_t<T_>>();
        }

        if (wrapperType == VariantWrapperType::LVALUE_REF
            || wrapperType == VariantWrapperType::RVALUE_REF) {
            return m_Type.Equals<std::remove_const_t<std::remove_reference_t<T_>>&>();
        }

        if (wrapperType == VariantWrapperType::CONST_LVALUE_REF
            || wrapperType == VariantWrapperType::CONST_RVALUE_REF) {
            return m_Type.Equals<T_>();
        }
    }
    else if constexpr (std::is_rvalue_reference_v<T_>) {
        // Only rvalue refs can be retrieved as rvalue refs

        if (wrapperType == VariantWrapperType::VALUE) {
            return m_Type.Equals<std::remove_const_t<std::remove_reference_t<T_>>>();
        }

        if (wrapperType == VariantWrapperType::CONST_VALUE) {
            return m_Type.Equals<std::remove_reference_t<T_>>();
        }

        if (wrapperType == VariantWrapperType::LVALUE_REF) {
            return m_Type.Equals<std::remove_const_t<std::remove_reference_t<T_>>&>();
        }

        if (wrapperType == VariantWrapperType::CONST_LVALUE_REF) {
            return m_Type.Equals<std::remove_reference_t<T_>&>();
        }

        if (wrapperType == VariantWrapperType::RVALUE_REF) {
            return m_Type.Equals<std::remove_const_t<std::remove_reference_t<T_>>&&>();
        }

        if (wrapperType == VariantWrapperType::CONST_RVALUE_REF) {
            return m_Type.Equals<std::remove_reference_t<T_>&&>();
        }
    }
    else if constexpr (std::is_pointer_v<T_>) {
        if (wrapperType == VariantWrapperType::POINTER) {
            return m_Type.Equals<std::remove_const_t<std::remove_volatile_t<std::remove_pointer_t<T_>>>*>();
        }

        if (wrapperType == VariantWrapperType::CONST_POINTER) {
            return m_Type.Equals<std::remove_volatile_t<std::remove_pointer_t<T_>>*>();
        }
    }
    else {
        if (wrapperType == VariantWrapperType::VALUE) {
            return m_Type.Equals<std::remove_const_t<T_>>();
        }
        if (wrapperType == VariantWrapperType::CONST_VALUE) {
            return m_Type.Equals<T_>();
        }
        if (wrapperType == VariantWrapperType::LVALUE_REF) {
            return m_Type.Equals<std::remove_const_t<T_>&>();
        }
        if (wrapperType == VariantWrapperType::CONST_LVALUE_REF) {
            return m_Type.Equals<T_&>();
        }
    }

    return false;
}

template <typename T_>
Result<void> Variant::CanGetWithError() const {
    if (detail::VariantMatcher<T_, >)

    // if (CanGet<T_>()) {
    //     return {};
    // }

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType(type, passed_type) };
}

template <typename T_>
    requires (!std::is_reference_v<T_> && !std::is_pointer_v<T_>)
Result<std::remove_volatile_t<T_>&> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    // const detail::VariantWrapperType wrapperType = m_Base->GetType();
    //
    // if (wrapperType == detail::VariantWrapperType::VALUE
    //     || wrapperType == detail::VariantWrapperType::LVALUE_REF) {
    //     return static_cast<detail::VariantWrapper<std::remove_const_t<T_>&>*>(m_Base.get())->GetValue();
    // }
    //
    // if (wrapperType == detail::VariantWrapperType::CONST_VALUE
    //     || wrapperType == detail::VariantWrapperType::CONST_LVALUE_REF) {
    //     return static_cast<detail::VariantWrapper<T_&>*>(m_Base.get())->GetValue();
    // }
    
    return { RESULT_ERROR(), "unreachable" };
}

template <typename T_>
    requires (std::is_lvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    return static_cast<detail::VariantWrapper<std::remove_volatile_t<std::remove_reference_t<T_>>&>*>(m_Base.get())->GetValue();
}

template <typename T_>
    requires (std::is_rvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    using ResultT = std::remove_volatile_t<std::remove_reference_t<T_>>&&;
    return std::forward<ResultT>(static_cast<detail::VariantWrapper<ResultT>*>(m_Base.get())->GetValue());
}

template <typename T_>
    requires (std::is_pointer_v<T_>)
Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    return static_cast<detail::VariantWrapper<std::remove_volatile_t<std::remove_pointer_t<T_>>*>*>(m_Base.get())->GetValue();
}
}
