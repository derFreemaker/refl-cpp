#pragma once

namespace ReflCpp {

template <typename Field_>
struct FieldTraits;

namespace detail {
template <bool IsStatic_, bool IsConst_, typename C, typename T>
struct FieldTraitsBase {
    static constexpr bool IsStatic = IsStatic_;
    static constexpr bool IsConst = IsConst_;
    
    using ClassType = C;
    using Type = T;
};
}

template <typename T>
struct FieldTraits<T*>
    : public detail::FieldTraitsBase<true, false, void, T> {};

template <typename C, typename T>
struct FieldTraits<T C::*>
    : public detail::FieldTraitsBase<false, false, C, T> {};

template <typename C, typename T>
struct FieldTraits<const T C::*>
    : public detail::FieldTraitsBase<false, true, C, T> {};

}