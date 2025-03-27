#pragma once

#include "refl-cpp/variant.hpp"

#include "refl-cpp/reflect.hpp"

namespace ReflCpp {
struct VoidVariantWrapper final : public detail::VariantWrapper<void> {
    void GetValue() override {}
};

template <typename T_>
struct ValueVariantWrapper final : public detail::VariantWrapper<const T_&> {
private:
    T_ m_Value;

public:
    ValueVariantWrapper(const T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct LValueRefVariantWrapper final : public detail::VariantWrapper<T_&> {
private:
    T_& m_Value;

public:
    LValueRefVariantWrapper(T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    T_& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct ConstLValueRefVariantWrapper final : public detail::VariantWrapper<const T_&> {
private:
    const T_& m_Value;

public:
    ConstLValueRefVariantWrapper(const T_& value)
        : m_Value(value) {}


    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct RValueRefVariantWrapper final : public detail::VariantWrapper<T_&&> {
private:
    T_&& m_Value;

public:
    RValueRefVariantWrapper(T_&& value)
        : m_Value(std::move(value)) {}

    [[nodiscard]]
    T_&& GetValue() override {
        return std::move(m_Value);
    }
};

template <typename T_>
struct ConstRValueRefVariantWrapper final : public detail::VariantWrapper<const T_&&> {
private:
    const T_&& m_Value;

public:
    ConstRValueRefVariantWrapper(const T_&& value)
        : m_Value(std::move(value)) {}


    [[nodiscard]]
    const T_&& GetValue() override {
        return std::move(m_Value);
    }
};

template <typename T_>
struct PointerVariantWrapper final : public detail::VariantWrapper<T_*&> {
private:
    T_* m_Value;

public:
    PointerVariantWrapper(T_* value)
        : m_Value(value) {}

    [[nodiscard]]
    T_*& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct ConstPointerVariantWrapper final : public detail::VariantWrapper<const T_*&> {
private:
    const T_* m_Value;

public:
    ConstPointerVariantWrapper(const T_* value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_*& GetValue() override {
        return m_Value;
    }
};

inline Variant::Variant()
    : m_Base(std::make_shared<VoidVariantWrapper>()),
      m_Type(ReflectID<void>().Value()) {}

template <typename T_, typename CleanT>
auto MakeValueWrapper(T_&& data) {
    if constexpr (std::is_copy_constructible_v<T_>) {
        return std::make_shared<ValueVariantWrapper<CleanT>>(data);
    }
    else {
        return std::make_shared<ConstLValueRefVariantWrapper<CleanT>>(data);
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
auto MakeLValueRefWrapper(T_&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstLValueRefVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<LValueRefVariantWrapper<CleanT_>>(data);
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
auto MakeRValueRefWrapper(T_&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstRValueRefVariantWrapper<CleanT_>>(std::forward<T_>(data));
    }
    else {
        return std::make_shared<RValueRefVariantWrapper<CleanT_>>(std::forward<T_>(data));
    }
}

template <typename T_, bool IsConst_ = false, typename CleanT_>
auto MakePointerWrapper(T_&& data) {
    if constexpr (IsConst_) {
        return std::make_shared<ConstPointerVariantWrapper<CleanT_>>(data);
    }
    else {
        return std::make_shared<PointerVariantWrapper<CleanT_>>(data);
    }
}

template <typename T_>
std::shared_ptr<detail::VariantBase> MakeWrapper(T_&& data) {
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

template <typename T_>
Variant Variant::Create(T_&& data) {
    constexpr bool isConst = std::is_const_v<T_>;
    return Variant(MakeWrapper(std::forward<T_>(data)), ReflectID<T_>().Value(), isConst);
}

inline FormattedError Variant::CanNotGetFromVariantWithType(const std::string_view& desc, const Type& type, const Type& passed_type) {
    return FormattedError {
        "cannot get {0} from Variant ({1}) with passed type: {2}",
        desc,
        type.Dump(),
        passed_type.Dump()
    };
}

// template <typename T_>
//     requires (!std::is_const_v<T_>)
// Result<make_lvalue_reference_t<T_>> Variant::GetRef() const {
//     TRY(CheckVoid());
//
//     using Traits = TypeTraits<T_>;
//
//     if (m_IsConst) {
//         return { RESULT_ERROR(), "cannot get modifiable reference to constant: {0} (use 'GetConstRef' for a constant reference)", TRY(m_Type.GetType()).Dump() };
//     }
//
//     if (m_Type == TRY(ReflectID<typename Traits::Type>())) {
//         return static_cast<detail::VariantWrapper<make_lvalue_reference_t<T_>>*>(m_Base.get())->GetValue();
//     }
//
//     const Type& type = TRY(m_Type.GetType());
//     const Type& passed_type = TRY(Reflect<T_>());
//     return { RESULT_ERROR(), CanNotGetFromVariantWithType("reference", type, passed_type) };
// }
//
// template <typename T_>
//     requires (std::is_const_v<T_>)
// Result<make_lvalue_reference_t<T_>> Variant::GetConstRef() const {
//     TRY(CheckVoid());
//
//     using Traits = TypeTraits<T_>;
//
//     if (m_Type == TRY(ReflectID<typename Traits::Type>())) {
//         return static_cast<detail::VariantWrapper<make_lvalue_reference_t<T_>>*>(m_Base.get())->GetValue();
//     }
//
//     const Type& type = TRY(m_Type.GetType());
//     const Type& passed_type = TRY(Reflect<T_>());
//     return { RESULT_ERROR(), CanNotGetFromVariantWithType("constant reference", type, passed_type) };
// }
//
//
// template <typename T_>
//     requires (!std::is_pointer_v<T_>)
// Result<make_const<std::remove_reference_t<T_>>&> Variant::GetValue() const {
//     TRY(CheckVoid());
//
//     using Traits = TypeTraits<T_>;
//
//     const TypeID passed_type_id = TRY(ReflectID<typename Traits::Type>());
//     if (m_Type == passed_type_id) {
//         return static_cast<detail::VariantWrapper<make_lvalue_reference_t<T_>>*>(m_Base.get())->GetValue();
//     }
//
//     using Type_ = std::remove_reference_t<T_>;
//
//     if (m_IsConst && m_Type == TRY(ReflectID<make_const<Type_>>())) {
//         return static_cast<detail::VariantWrapper<const Type_&>*>(m_Base.get())->GetValue();
//     }
//
//     if (m_Type == TRY(ReflectID<make_const<Type_>&>())) {
//         return static_cast<detail::VariantWrapper<const Type_&>*>(m_Base.get())->GetValue();
//     }
//
//     const Type& type = TRY(m_Type.GetType());
//     const Type& passed_type = TRY(Reflect<T_>());
//     return { RESULT_ERROR(), CanNotGetFromVariantWithType("value", type, passed_type) };
// }
//
// template <typename T_>
//     requires (std::is_pointer_v<T_>)
// Result<add_const_to_pointer_type_t<T_>&> Variant::GetValue() const {
//     TRY(CheckVoid());
//
//     if (m_Type == TRY(ReflectID<T_>())) {
//         return const_cast<add_const_to_pointer_type_t<T_>&>(static_cast<detail::VariantWrapper<T_&>*>(m_Base.get())->GetValue());
//     }
//
//     if (!std::is_const_v<T_> && m_Type == TRY(ReflectID<add_const_to_pointer_type_t<T_>>())) {
//         return const_cast<add_const_to_pointer_type_t<T_>&>(static_cast<detail::VariantWrapper<T_&>*>(m_Base.get())->GetValue());
//     }
//
//     const Type& type = TRY(m_Type.GetType());
//     if (m_IsConst
//         && type.GetFlags().Has(TypeFlags::IsPointer)
//         && type.HasInner(TRY(ReflectID<const std::remove_pointer_t<T_>>()))) {
//         return static_cast<detail::VariantWrapper<add_const_to_pointer_type_t<T_>&>*>(m_Base.get())->GetValue();
//     }
//
//     const Type& passed_type = TRY(Reflect<T_>());
//     return { RESULT_ERROR(), CanNotGetFromVariantWithType("value", type, passed_type) };
// }

template <typename T_>
    requires (!std::is_reference_v<T_> && !std::is_pointer_v<T_>)
Result<make_const<T_>&> Variant::Get() const {
    TRY(CheckVoid());

    const TypeID passed_type_id = TRY(ReflectID<T_>());
    if (m_Type == passed_type_id) {
        return static_cast<detail::VariantWrapper<
            make_const<T_>&>*>(m_Base.get())->GetValue();
    }

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType("value", type, passed_type) };
}

template <typename T_>
    requires (std::is_lvalue_reference_v<T_> && std::is_const_v<std::remove_reference_t<T_>>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Variant::Get() const {
    TRY(CheckVoid());

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType("value", type, passed_type) };
}

template <typename T_>
    requires (std::is_lvalue_reference_v<T_> && !std::is_const_v<std::remove_reference_t<T_>>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Variant::Get() const {
    TRY(CheckVoid());

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType("value", type, passed_type) };
}

template <typename T_>
    requires (std::is_rvalue_reference_v<T_> && std::is_const_v<std::remove_reference_t<T_>>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Variant::Get() const {
    TRY(CheckVoid());

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType("value", type, passed_type) };
}

template <typename T_>
    requires (std::is_rvalue_reference_v<T_> && !std::is_const_v<std::remove_reference_t<T_>>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Variant::Get() const {
    TRY(CheckVoid());

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType("value", type, passed_type) };
}

template <typename T_>
    requires (!std::is_reference_v<T_> && std::is_pointer_v<T_> && std::is_const_v<std::remove_pointer_t<T_>>)
Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Variant::Get() const {
    TRY(CheckVoid());

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType("value", type, passed_type) };
}

template <typename T_>
    requires (!std::is_reference_v<T_> && std::is_pointer_v<T_> && !std::is_const_v<std::remove_pointer_t<T_>>)
Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Variant::Get() const {
    TRY(CheckVoid());

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType("value", type, passed_type) };
}
}
