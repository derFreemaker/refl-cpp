#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp::detail {
template <typename R>
    requires (!std::is_pointer_v<R> && !std::is_reference_v<R>)
struct VariantMatcher<VariantWrapperType::CONST_VALUE, const R> {
    static bool Match(const TypeID type) {
        return type.Equals<const R>();
    }

    static const R& Get(VariantBase* base) {
        return static_cast<VariantWrapper<const R&>*>(base)->GetValue();
    }
};

template <typename R>
struct VariantMatcher<VariantWrapperType::CONST_VALUE, const R&> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<const R>();
    }

    static const R& Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<const R&>*>(base)->GetValue();
    }
};
}
