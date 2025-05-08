#pragma once

#include "refl-cpp/variant.hpp"

template <typename R>
struct ReflCpp::detail::VariantMatcher<ReflCpp::detail::VariantWrapperType::CONST_RVALUE_REF, const R&&> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<const R&&>();
    }

    static const R&& Get(VariantBase* base) noexcept {
        return std::forward<const R>(static_cast<VariantWrapper<const R&&>*>(base)->GetValue());
    }
};
