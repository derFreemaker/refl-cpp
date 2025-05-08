#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp::detail {
template <typename R>
struct VariantMatcher<VariantWrapperType::RVALUE_REF, R&&> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<R&&>();
    }

    static R&& Get(VariantBase* base) noexcept {
        return std::forward<R>(static_cast<VariantWrapper<R&&>*>(base)->GetValue());
    }
};

template <typename R>
struct VariantMatcher<VariantWrapperType::RVALUE_REF, const R&&> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<R&&>();
    }

    static const R&& Get(VariantBase* base) noexcept {
        return std::forward<R>(static_cast<VariantWrapper<R&&>*>(base)->GetValue());
    }
};
}
