#pragma once
#include <stdexcept>

#include <fmt/core.h>

#include "refl-cpp/reflect.hpp"
#include "refl-cpp/variant.hpp"

namespace ReflCpp {
template <typename T_>
    requires detail::LimitVariant<T_>
Result<T_&> Variant::GetRef() const {
    CheckVoid();

    if (m_IsConst && !std::is_const_v<T_>) {
        return { Error, "cannot get reference to constant" };
    }

    auto passed_type_id = TRY(ReflectID<T_>());
    if (m_Type == passed_type_id) {
        return { Ok, static_cast<VariantWrapper<T_&>*>(m_Base.get())->GetValue() };
    }

    return { Error, "passed type is not the same as the stored type" };
}

template <typename T_>
    requires (detail::LimitVariant<T_> && !std::is_pointer_v<T_>)
Result<const T_&> Variant::GetValue() const {
    CheckVoid();

    auto type_id = TRY(ReflectID<T_>());
    if (type_id == m_Type) {
        return { Ok, static_cast<VariantWrapper<T_&>*>(m_Base.get())->GetValue() };
    }

    if (m_IsConst
        && m_Type.GetType().Value().GetFlags().Has(TypeFlags::IsConst)
        && m_Type.GetType().Value().HasInner(ReflectID<T_>())) {
        return { Ok, static_cast<VariantWrapper<const T_&>*>(m_Base.get())->GetValue() };
    }

    return {
        Error,
        "passed type '{0}' is not the same as the stored type '{1}'",
        Reflect<T_>().Value().Dump(),
        m_Type.GetType().Value().Dump()
    };
}

template <typename T_>
    requires (detail::LimitVariant<T_> && std::is_pointer_v<T_>)
Result<const std::remove_pointer_t<T_>*&> Variant::GetValue() const {
    CheckVoid();

    using return_type = const std::remove_pointer_t<T_>*;

    auto type_id = TRY(ReflectID<T_>());
    if (type_id == m_Type) {
        return { Ok, const_cast<return_type&>(static_cast<VariantWrapper<T_&>*>(m_Base.get())->GetValue()) };
    }

    if (m_IsConst
        && m_Type.GetType().Value().GetFlags().Has(TypeFlags::IsPointer)
        && m_Type.GetType().Value().HasInner(ReflectID<const std::remove_pointer_t<T_>>())) {
        return { Ok, static_cast<VariantWrapper<return_type&>*>(m_Base.get())->GetValue() };
    }

    return {
        Error,
        "passed type '{0}' is not the same as the stored type '{1}'",
        Reflect<T_>().Value().Dump(),
        m_Type.GetType().Value().Dump()
    };
}
}
