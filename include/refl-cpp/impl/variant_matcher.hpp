#pragma once

#include "../variant.hpp"

namespace ReflCpp::detail {
template <VariantWrapperType Type, typename R>
struct VariantMatcher {
    static bool Match(const TypeID _) {
#ifndef NDEBUG
        throw std::logic_error("Not implemented\n" + to_string(std::stacktrace::current()));
#else
        return false;
#endif
    }
};

template <typename R_>
struct VariantMatcher<VariantWrapperType::VOID, R_> {
    static bool Match(const TypeID _) {
        return false;
    }
};

template <typename R_>
    requires (!std::is_pointer_v<R_> && !std::is_reference_v<R_>)
struct VariantMatcher<VariantWrapperType::VALUE, R_> {
    using ResultT = R_&;

    static bool Match(const TypeID type) {
        return type.Equals<R_>();
    }

    static ResultT Get(VariantBase* base) {
        return static_cast<VariantWrapper<ResultT>*>(base)->GetValue();
    }
};

template <typename R_>
    requires (!std::is_pointer_v<R_> && !std::is_reference_v<R_>)
struct VariantMatcher<VariantWrapperType::VALUE, const R_> {
    using ResultT = R_&;

    static bool Match(const TypeID type) {
        return type.Equals<R_>()
            || type.Equals<const R_>();
    }

    static ResultT Get(VariantBase* base) {
        return static_cast<VariantWrapper<ResultT>*>(base)->GetValue();
    }
};

template <typename R_>
struct VariantMatcher<VariantWrapperType::VALUE, R_&> {
    using ResultT = R_&;

    static bool Match(const TypeID type) {
        return type.Equals<R_>();
    }

    static ResultT Get(VariantBase* base) {
        return static_cast<VariantWrapper<ResultT>*>(base)->GetValue();
    }
};

template <typename R_>
struct VariantMatcher<VariantWrapperType::VALUE, const R_&> {
    static bool Match(const TypeID type) {
        return type.Equals<R_>()
            || type.Equals<const R_>();
    }

    static const R_& Get(VariantBase* base) {
        return static_cast<VariantWrapper<const R_&>*>(base)->GetValue();
    }
};
}
