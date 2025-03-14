#pragma once
#include "variant_wrapper.hpp"

namespace ReflCpp {
namespace detail {
template <bool IsConst_, bool IsReference_, typename T_>
struct VariantTraitsBase {
    static constexpr bool IsConst = IsConst_;
    static constexpr bool IsReference = IsReference_;

    using Type = T_;
};
}

template <typename T_>
struct VariantTraits
    : detail::VariantTraitsBase<false, false, T_> {};

template <typename T_>
struct VariantTraits<const T_>
    : detail::VariantTraitsBase<true, false, T_> {};

template <typename T_>
struct VariantTraits<const T_&>
    : detail::VariantTraitsBase<true, true, T_> {};

template <typename T_>
struct VariantTraits<T_&>
    : detail::VariantTraitsBase<false, true, T_> {};
}
