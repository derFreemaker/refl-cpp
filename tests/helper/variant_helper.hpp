#pragma once

#include "refl-cpp/variant.hpp"

namespace ReflCpp {
struct VariantTestHelper {
    template <typename Wrapper_>
    static Wrapper_* GetWrapper(const Variant& variant) {
        return dynamic_cast<Wrapper_*>(variant.m_Base.get());
    }

    template <typename Wrapper_>
    static bool UsesWrapper(const Variant& variant) {
        return GetWrapper<Wrapper_>(variant) != nullptr;
    }
};
}
