#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp::detail {
template <typename R>
struct VariantMatcher<VariantWrapperType::POINTER, R*> {
    static bool Match(const TypeID type) {
        return type.Equals<R*>();
    }

    static R* Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<R*>*>(base)->GetValue();
    }
};

template <typename R>
struct VariantMatcher<VariantWrapperType::POINTER, const R*> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<R*>();
    }

    static const R* Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<R*>*>(base)->GetValue();
    }
};
}
