#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp::detail {
template <typename R_>
struct VariantMatcher<VariantWrapperType::RVALUE_REF, R_&&> {
    static bool Match(const TypeID type) {
        return type.Equals<R_&&>();
    }

    static R_&& Get(VariantBase* base) {
        return std::forward<R_>(static_cast<VariantWrapper<R_&&>*>(base)->GetValue());
    }
};

template <typename R_>
struct VariantMatcher<VariantWrapperType::RVALUE_REF, const R_&&> {
    static bool Match(const TypeID type) {
        return type.Equals<R_&&>();
    }

    static R_&& Get(VariantBase* base) {
        return std::forward<R_>(static_cast<VariantWrapper<R_&&>*>(base)->GetValue());
    }
};
}
