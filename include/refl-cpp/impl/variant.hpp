#pragma once

#include "refl-cpp/variant.hpp"

#include "refl-cpp/reflect.hpp"

namespace ReflCpp {

//TODO: rewrite whole checking if type matches system

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
    if constexpr (!std::is_reference_v<T_> && !std::is_pointer_v<T_>) {
        switch (m_Base->GetType()) {
            case detail::VariantWrapperType::VALUE:
                return true;

            case detail::VariantWrapperType::CONST_VALUE:
                return std::is_const_v<T_>;

            default:
                break;
        }
    }
    else if constexpr (std::is_lvalue_reference_v<T_>) {
        switch (m_Base->GetType()) {
            case detail::VariantWrapperType::VALUE:
            case detail::VariantWrapperType::LVALUE_REF:
            case detail::VariantWrapperType::RVALUE_REF:
                return true;

            case detail::VariantWrapperType::CONST_VALUE:
            case detail::VariantWrapperType::CONST_LVALUE_REF:
            case detail::VariantWrapperType::CONST_RVALUE_REF:
                return std::is_const_v<std::remove_reference_t<T_>>;

            default:
                break;
        }
    }
    else if constexpr (std::is_rvalue_reference_v<T_>) {
        switch (m_Base->GetType()) {
            case detail::VariantWrapperType::LVALUE_REF:
            case detail::VariantWrapperType::RVALUE_REF:
                return true;

            case detail::VariantWrapperType::CONST_LVALUE_REF:
            case detail::VariantWrapperType::CONST_RVALUE_REF:
                return std::is_const_v<std::remove_reference_t<T_>>;

            default:
                break;
        }
    }
    else if constexpr (std::is_pointer_v<T_>) {
        switch (m_Base->GetType()) {
            case detail::VariantWrapperType::POINTER:
                return true;

            case detail::VariantWrapperType::CONST_POINTER:
                return std::is_const_v<std::remove_pointer_t<T_>>;

            default:
                break;
        }
    }

    return false;
}

template <typename T_>
Result<void> Variant::CanGetWithError() const {
    if (CanGet<T_>()) {
        return {};
    }

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType(type, passed_type) };
}

template <typename T_>
    requires (!std::is_reference_v<T_> && !std::is_pointer_v<T_>)
Result<std::remove_volatile_t<T_>&> Variant::Get() const {
    TRY(CheckVoid());

    switch (m_Base->GetType()) {
        case detail::VariantWrapperType::VALUE:
            if (m_Type == TRY(ReflectID<std::remove_const_t<T_>>())) {
                return static_cast<detail::VariantWrapper<std::remove_const_t<T_>&>*>(m_Base.get())->GetValue();
            }

        case detail::VariantWrapperType::CONST_VALUE: {
            if (m_Type == TRY(ReflectID<T_>())) {
                return static_cast<detail::VariantWrapper<T_&>*>(m_Base.get())->GetValue();
            }
        }

        default:
            const Type& type = TRY(m_Type.GetType());
            const Type& passed_type = TRY(Reflect<T_>());
            return { RESULT_ERROR(), CanNotGetFromVariantWithType(type, passed_type) };
    }
}

template <typename T_>
    requires (std::is_lvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Variant::Get() const {
    TRY(CheckVoid());

    using CleanT_ = std::remove_volatile_t<std::remove_reference_t<T_>>&;

    switch (m_Base->GetType()) {
        case detail::VariantWrapperType::VALUE:
            if (m_Type == TRY(ReflectID<std::remove_const_t<std::remove_reference_t<CleanT_>>>())) {
                return static_cast<detail::VariantWrapper<std::remove_const_t<CleanT_>>*>(m_Base.get())->GetValue();
            }
            break;

        case detail::VariantWrapperType::CONST_VALUE:
            if (m_Type == TRY(ReflectID<std::remove_reference_t<CleanT_>>())) {
                return static_cast<detail::VariantWrapper<CleanT_>*>(m_Base.get())->GetValue();
            }
            break;

        case detail::VariantWrapperType::LVALUE_REF:
            case detail::VariantWrapperType::CONST_LVALUE_REF:
            if (m_Type == TRY(ReflectID<T_>())
                || m_Type == TRY(ReflectID<std::remove_const_t<std::remove_reference_t<T_>>&>())) {
                return static_cast<detail::VariantWrapper<CleanT_>*>(m_Base.get())->GetValue();
            }
            break;
        
        default:
            break;
    }

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType(type, passed_type) };
}

template <typename T_>
    requires (std::is_rvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Variant::Get() const {
    TRY(CheckVoid());

    using CleanT_ = std::remove_volatile_t<std::remove_reference_t<T_>>&&;

    if (m_Type == TRY(ReflectID<T_>())
        || m_Type == TRY(ReflectID<std::remove_const_t<std::remove_reference_t<T_>>&&>())) {
        return std::forward<CleanT_>(static_cast<detail::VariantWrapper<CleanT_>*>(m_Base.get())->GetValue());
    }

    if (m_Type == TRY(ReflectID<std::remove_reference_t<CleanT_>>())
        || m_Type == TRY(ReflectID<std::remove_const_t<std::remove_reference_t<CleanT_>>>())) {
        return std::forward<CleanT_>(TRY(Get<std::remove_reference_t<CleanT_>>()));
    }

    if (m_Type == TRY(ReflectID<std::remove_reference_t<CleanT_>&>())
        || m_Type == TRY(ReflectID<std::remove_const_t<std::remove_reference_t<CleanT_>>&>())) {
        return std::forward<CleanT_>(TRY(Get<std::remove_reference_t<CleanT_>&>()));
    }

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType(type, passed_type) };
}

template <typename T_>
    requires (std::is_pointer_v<T_>)
Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Variant::Get() const {
    TRY(CheckVoid());

    using CleanT_ = std::remove_volatile_t<std::remove_pointer_t<T_>>*;

    switch (m_Base->GetType()) {
        case detail::VariantWrapperType::POINTER:
            if (m_Type == TRY(ReflectID<remove_const_from_pointer_t<CleanT_>>())) {
                return static_cast<detail::VariantWrapper<remove_const_from_pointer_t<CleanT_>>*>(m_Base.get())->GetValue();
            }

        case detail::VariantWrapperType::CONST_POINTER:
            if (m_Type == TRY(ReflectID<CleanT_>())) {
                return static_cast<detail::VariantWrapper<CleanT_>*>(m_Base.get())->GetValue();
            }

        default:
            const Type& type = TRY(m_Type.GetType());
            const Type& passed_type = TRY(Reflect<T_>());
            return { RESULT_ERROR(), CanNotGetFromVariantWithType(type, passed_type) };
    }
}
}
