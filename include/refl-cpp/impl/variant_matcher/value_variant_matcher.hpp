#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp::detail {
template <typename R>
    requires (!std::is_pointer_v<R> && !std::is_reference_v<R>)
struct VariantMatcher<VariantWrapperType::VALUE, R> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<R>();
    }

    static R& Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<R&>*>(base)->GetValue();
    }
};

template <typename R>
    requires (!std::is_pointer_v<R> && !std::is_reference_v<R>)
struct VariantMatcher<VariantWrapperType::VALUE, const R> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<R>();
    }

    static const R& Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<R&>*>(base)->GetValue();
    }
};

template <typename R>
struct VariantMatcher<VariantWrapperType::VALUE, R&> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<R>();
    }

    static R& Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<R&>*>(base)->GetValue();
    }
};

template <typename R>
struct VariantMatcher<VariantWrapperType::VALUE, const R&> {
    static bool Match(const TypeID type) noexcept {
        return type.Equals<R>();
    }

    static const R& Get(VariantBase* base) noexcept {
        return static_cast<VariantWrapper<R&>*>(base)->GetValue();
    }
};
}
