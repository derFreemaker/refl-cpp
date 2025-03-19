#pragma once
#include <stdexcept>

#include <fmt/core.h>

#include "refl-cpp/reflect.hpp"
#include "refl-cpp/variant.hpp"

namespace ReflCpp {
template <typename T_>
    requires detail::LimitVariant<T_>
T_& Variant::GetRef() const {
    CheckVoid();

    if (m_IsConst && !std::is_const_v<T_>) {
        throw std::logic_error("cannot get reference to constant");
    }

    if (m_Type == ReflectID<T_>()) {
        return static_cast<VariantWrapper<T_&>*>(m_Base.get())->GetValue();
    }

    throw std::invalid_argument("passed type is not the same as the stored type");
}

template <typename T_>
    requires (detail::LimitVariant<T_> && !std::is_pointer_v<T_>)
const T_& Variant::GetValue() const {
    CheckVoid();

    auto type_id = ReflectID<T_>();
    if (type_id == m_Type) {
        return static_cast<VariantWrapper<T_&>*>(m_Base.get())->GetValue();
    }

    if (m_IsConst
        && m_Type.GetType().Value().GetFlags().Has(TypeFlags::IsConst)
        && m_Type.GetType().Value().HasInner(ReflectID<T_>())) {
        return static_cast<VariantWrapper<const T_&>*>(m_Base.get())->GetValue();
    }

    throw std::invalid_argument(fmt::format(
        "passed type '{0}' is not the same as the stored type '{1}'",
        Reflect<T_>().Value().Dump(), m_Type.GetType().Value().Dump()
    ));
}

template <typename T_>
    requires (detail::LimitVariant<T_> && std::is_pointer_v<T_>)
const std::remove_pointer_t<T_>*& Variant::GetValue() const {
    CheckVoid();

    using return_type = const std::remove_pointer_t<T_>*;

    auto type_id = ReflectID<T_>();
    if (type_id == m_Type) {
        return const_cast<return_type&>(static_cast<VariantWrapper<T_&>*>(m_Base.get())->GetValue());
    }

    if (m_IsConst
        && m_Type.GetType().Value().GetFlags().Has(TypeFlags::IsPointer)
        && m_Type.GetType().Value().HasInner(ReflectID<const std::remove_pointer_t<T_>>())) {
        return static_cast<VariantWrapper<return_type&>*>(m_Base.get())->GetValue();
    }

    throw std::invalid_argument(
        std::string("passed type: '")
        + std::string(TRY(Reflect<T_>()).Dump())
        + std::string("' is not the same as the stored type '")
        + std::string(TRY(m_Type.GetType()).Dump())
        + std::string("'")
    );
}
}
