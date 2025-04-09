#pragma once

#include "refl-cpp/variant.hpp"

template <typename R_>
struct ReflCpp::detail::VariantMatcher<ReflCpp::detail::VariantWrapperType::CONST_RVALUE_REF, const R_&&> {
    static bool Match(const TypeID type) {
        return type.Equals<const R_&&>();
    }

    static const R_&& Get(VariantBase* base) {
        return std::forward<const R_>(static_cast<VariantWrapper<const R_&&>*>(base)->GetValue());
    }
};
