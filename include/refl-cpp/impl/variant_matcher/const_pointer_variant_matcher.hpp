#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp::detail {
template <typename R_>
struct VariantMatcher<VariantWrapperType::CONST_POINTER, const R_*> {
    static bool Match(const TypeID type) {
        return type.Equals<const R_*>();
    }

    static const R_* Get(VariantBase* base) {
        return static_cast<VariantWrapper<R_*>*>(base)->GetValue();
    }
};
}
