#pragma once

namespace ReflCpp {

template <typename Field_>
struct FieldTraits;

namespace detail {
template <bool IsStatic_, bool IsConst_, typename C_, typename T_>
struct FieldTraitsBase {
    static constexpr bool IsStatic = IsStatic_;
    static constexpr bool IsConst = IsConst_;
    
    using ClassType = C_;
    using Type = T_;
};
}

template <typename T_>
struct FieldTraits<T_*>
    : public detail::FieldTraitsBase<true, false, void, T_> {};

template <typename C_, typename T_>
struct FieldTraits<T_ C_::*>
    : public detail::FieldTraitsBase<false, false, C_, T_> {};

template <typename C_, typename T_>
struct FieldTraits<const T_ C_::*>
    : public detail::FieldTraitsBase<false, true, C_, T_> {};

}