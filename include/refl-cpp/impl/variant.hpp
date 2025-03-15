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
    
    if (ReflectID<T_>() == m_Type) {
        return static_cast<VariantWrapper<T_&>*>(m_Base.get())->GetValue();
    }

    if (m_IsConst && ReflectID<const T_>() == m_Type) {
        return static_cast<VariantWrapper<const T_&>*>(m_Base.get())->GetValue();
    }

    throw std::invalid_argument("passed type is not the same as the stored type");
}
}
