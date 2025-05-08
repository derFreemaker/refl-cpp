#pragma once

#include "refl-cpp/variant.hpp"
#include "refl-cpp/impl/variant.hpp"

namespace ReflCpp::testing {
struct VariantTestHelper {
    template <typename Wrapper_>
    static Wrapper_* GetWrapper(const Variant& variant) {
        return dynamic_cast<Wrapper_*>(variant.base_.get());
    }

    template <typename Wrapper_>
    static bool UsesWrapper(const Variant& variant) {
        return GetWrapper<Wrapper_>(variant) != nullptr;
    }
};
}
