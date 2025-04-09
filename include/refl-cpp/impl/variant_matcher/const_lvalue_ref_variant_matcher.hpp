#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp::detail {
template <typename R_>
    requires (!std::is_pointer_v<R_> && !std::is_reference_v<R_>)
struct VariantMatcher<VariantWrapperType::CONST_LVALUE_REF, const R_> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<const R_&>();
    }

    static const R_& Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<const R_&>*>(base)->GetValue();
    }
};

template <typename R_>
struct VariantMatcher<VariantWrapperType::CONST_LVALUE_REF, const R_&> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<const R_&>();
    }

    static const R_& Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<const R_&>*>(base)->GetValue();
    }
};
}
