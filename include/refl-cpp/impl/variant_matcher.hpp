#pragma once

#include "refl-cpp/variant.hpp"

#include "refl-cpp/impl/variant_matcher/value_variant_matcher.hpp"
#include "refl-cpp/impl/variant_matcher/const_value_variant_matcher.hpp"
#include "refl-cpp/impl/variant_matcher/lvalue_ref_variant_matcher.hpp"

// There is technically no need for this since we just default to 'false' anyway
template <typename R_>
struct ReflCpp::detail::VariantMatcher<ReflCpp::detail::VariantWrapperType::VOID, R_> {
    static bool Match(const TypeID) {
        return false;
    }
};
