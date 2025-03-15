#pragma once
#include <stdexcept>

#include "refl-cpp/reflect.hpp"
#include "refl-cpp/variant.hpp"

namespace ReflCpp {
template <typename T_> requires detail::LimitVariant<T_>
const T_& Variant::GetValue() const {
    if (m_Type == ReflectID<void>()) {
        throw std::logic_error("cannot get reference to void variant");
    }

    const auto passed_type_id = ReflectID<T_>();
    if (passed_type_id == m_Type) {
        return static_cast<VariantWrapper<T_&>*>(m_Base.get())->GetValue();
    }

    if (const auto& type = Reflect(m_Type); type.HasInner() && type.IsConst() && type.GetInner().GetID() == passed_type_id) {
        return static_cast<VariantWrapper<const T_&>*>(m_Base.get())->GetValue();
    }
    
    throw std::invalid_argument("passed type is not the same as the stored type");
}
}
