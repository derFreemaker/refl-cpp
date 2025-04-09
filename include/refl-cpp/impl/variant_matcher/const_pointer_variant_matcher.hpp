#pragma once

#include "refl-cpp/variant.hpp"

template <typename R_>
struct ReflCpp::detail::VariantMatcher<ReflCpp::detail::VariantWrapperType::CONST_POINTER, const R_*> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<const R_*>();
    }

    static const R_* Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<R_*>*>(base)->GetValue();
    }
};
