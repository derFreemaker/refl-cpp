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
    const T_ m_Value;

public:
    ValueVariantWrapper(const T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct RefVariantWrapper final : public detail::VariantWrapper<T_&> {
private:
    T_& m_Value;

public:
    RefVariantWrapper(T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    T_& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct ConstRefVariantWrapper final : public detail::VariantWrapper<const T_&> {
private:
    const T_& m_Value;

public:
    ConstRefVariantWrapper(const T_& value)
        : m_Value(value) {}


    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
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
template <typename T_>

    requires (detail::BlockVariant<T_> && std::copy_constructible<T_>)
Variant::Variant(T_& data)
    : m_Base(std::make_shared<ValueVariantWrapper<T_>>(data)),
      m_Type(ReflectID<T_>().Value()) {}

template <typename T_>
    requires (detail::BlockVariant<T_> && std::copy_constructible<T_>)
Variant::Variant(const T_& data)
    : m_Base(std::make_shared<ValueVariantWrapper<T_>>(data)),
      m_Type(ReflectID<const T_>().Value()),
      m_IsConst(true) {}

template <typename T_>
    requires (detail::BlockVariant<T_> && !std::copy_constructible<T_>)
Variant::Variant(T_& data)
    : m_Base(std::make_shared<RefVariantWrapper<T_>>(data)),
      m_Type(ReflectID<T_&>().Value()) {}

template <typename T_>
    requires (detail::BlockVariant<T_> && !std::copy_constructible<T_>)
Variant::Variant(const T_& data)
    : m_Base(std::make_shared<ConstRefVariantWrapper<T_>>(data)),
      m_Type(ReflectID<const T_&>().Value()),
      m_IsConst(true) {}

template <typename T_>
    requires (detail::BlockVariant<T_>)
Variant::Variant(T_* data)
    : m_Base(std::make_shared<PointerVariantWrapper<T_>>(data)),
      m_Type(ReflectID<T_*>().Value()) {}

template <typename T_>
    requires (detail::BlockVariant<T_>)
Variant::Variant(const T_* data)
    : m_Base(std::make_shared<ConstPointerVariantWrapper<T_>>(data)),
      m_Type(ReflectID<const T_*>().Value()),
      m_IsConst(true) {}

template <typename T_>
    requires (!std::is_same_v<T_, Variant>)
Result<make_lvalue_reference_t<T_>> Variant::GetRef() const {
    TRY(CheckVoid());

    if (m_IsConst && !std::is_const_v<std::remove_pointer_t<std::remove_reference_t<T_>>>) {
        return { RESULT_ERROR(), "cannot get modifiable reference to constant" };
    }

    const TypeID passed_type_id = TRY(ReflectID<T_>());
    if (m_Type == passed_type_id) {
        return { RESULT_OK(), static_cast<detail::VariantWrapper<std::remove_reference_t<T_>&>*>(m_Base.get())->GetValue() };
    }

    return { RESULT_ERROR(), "passed type is not the same as the stored type" };
}

template <typename T_>
    requires (!std::is_same_v<T_, Variant> && !std::is_pointer_v<T_>)
Result<const T_&> Variant::GetValue() const {
    TRY(CheckVoid());

    const TypeID passed_type_id = TRY(ReflectID<T_>());
    if (m_Type == passed_type_id) {
        return { RESULT_OK(), static_cast<detail::VariantWrapper<T_&>*>(m_Base.get())->GetValue() };
    }

    const Type& type = TRY(m_Type.GetType());
    if (m_IsConst
        && type.GetFlags().Has(TypeFlags::IsConst)
        && type.HasInner(passed_type_id)) {
        return { RESULT_OK(), static_cast<detail::VariantWrapper<const T_&>*>(m_Base.get())->GetValue() };
    }

    const Type& passed_type = TRY(Reflect<T_>());
    return {
        RESULT_ERROR(),
        "passed type '{0}' is not the same as the stored type '{1}'",
        passed_type.Dump(),
        type.Dump()
    };
}

template <typename T_>
    requires (!std::is_same_v<T_, Variant> && std::is_pointer_v<T_>)
Result<add_const_to_pointer_type_t<T_>&> Variant::GetValue() const {
    TRY(CheckVoid());

    if (m_Type == TRY(ReflectID<T_>())) {
        return { RESULT_OK(), const_cast<add_const_to_pointer_type_t<T_>&>(static_cast<detail::VariantWrapper<T_&>*>(m_Base.get())->GetValue()) };
    }

    const Type& type = TRY(m_Type.GetType());
    if (m_IsConst
        && type.GetFlags().Has(TypeFlags::IsPointer)
        && type.HasInner(TRY(ReflectID<const std::remove_pointer_t<T_>>()))) {
        return { RESULT_OK(), static_cast<detail::VariantWrapper<add_const_to_pointer_type_t<T_>&>*>(m_Base.get())->GetValue() };
    }

    const Type& passed_type = TRY(Reflect<T_>());
    return {
        RESULT_ERROR(),
        "passed type '{0}' is not the same as the stored type '{1}'",
        passed_type.Dump(),
        type.Dump()
    };
}
}
