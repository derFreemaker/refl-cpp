#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp::detail {
template <typename R_>
    requires (!std::is_pointer_v<R_> && !std::is_reference_v<R_>)
struct VariantMatcher<VariantWrapperType::VALUE, R_> {
    static bool Match(const TypeID type) {
        return type.Equals<R_>();
    }

    using ResultT = R_&;

    static ResultT Get(VariantBase* base) {
        return static_cast<VariantWrapper<ResultT>*>(base)->GetValue();
    }
};

template <typename R_>
    requires (!std::is_pointer_v<R_> && !std::is_reference_v<R_>)
struct VariantMatcher<VariantWrapperType::VALUE, const R_> {
    static bool Match(const TypeID type) {
        return type.Equals<R_>();
    }

    using ResultT = R_&;

    static ResultT Get(VariantBase* base) {
        return static_cast<VariantWrapper<ResultT>*>(base)->GetValue();
    }
};

template <typename R_>
struct VariantMatcher<VariantWrapperType::VALUE, R_&> {
    static bool Match(const TypeID type) {
        return type.Equals<R_>();
    }

    using ResultT = R_&;

    static ResultT Get(VariantBase* base) {
        return static_cast<VariantWrapper<ResultT>*>(base)->GetValue();
    }
};

template <typename R_>
struct VariantMatcher<VariantWrapperType::VALUE, const R_&> {
    static bool Match(const TypeID type) {
        return type.Equals<R_>();
    }

    using ResultT = const R_&;

    static ResultT Get(VariantBase* base) {
        return static_cast<VariantWrapper<ResultT>*>(base)->GetValue();
    }
};
}
