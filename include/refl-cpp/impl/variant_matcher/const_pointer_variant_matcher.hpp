#pragma once

#include "refl-cpp/variant.hpp"

template <typename R>
struct ReflCpp::detail::VariantMatcher<ReflCpp::detail::VariantWrapperType::CONST_POINTER, const R*> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<const R*>();
    }

    static const R* Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<R*>*>(base)->GetValue();
    }
};
