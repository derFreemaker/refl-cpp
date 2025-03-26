#pragma once

#include <type_traits>

namespace ReflCpp {
template <typename T_>
using make_const = std::add_const_t<std::remove_const_t<T_>>;

namespace detail {
template <typename T_>
struct add_const_to_pointer_type;

template <typename T_>
struct add_const_to_pointer_type<T_*> {
    using type = const T_*;
};

template <typename T_>
struct add_const_to_pointer_type<T_* const> {
    using type = const T_* const;
};

template <typename T_>
struct add_const_to_pointer_type<T_* volatile> {
    using type = const T_* volatile;
};

template <typename T_>
struct add_const_to_pointer_type<T_* const volatile> {
    using type = const T_* const volatile;
};
}

template <typename T_>
    requires std::is_pointer_v<T_>
struct add_const_to_pointer_type {
    using type = typename detail::add_const_to_pointer_type<T_>::type;
};

template <typename T_>
using add_const_to_pointer_type_t = typename add_const_to_pointer_type<T_>::type;

namespace detail {
template <typename T_>
struct make_lvalue_reference {
    using type = T_&;
};

template <typename T_>
struct make_lvalue_reference<T_&> {
    using type = T_&;
};
}

template <typename T_>
struct make_lvalue_reference {
    using type = typename detail::make_lvalue_reference<T_>::type;
};

template <typename T_>
using make_lvalue_reference_t = typename make_lvalue_reference<T_>::type;

namespace detail {
template <typename T_>
struct remove_all_pointers {
    using type = T_;
};

template <typename T_>
struct remove_all_pointers<T_* > {
    using type = typename remove_all_pointers<T_>::type;
};

template <typename T_>
struct remove_all_pointers<T_* const> {
    using type = typename remove_all_pointers<T_>::type;
};

template <typename T_>
struct remove_all_pointers<T_* volatile> {
    using type = typename remove_all_pointers<T_>::type;
};

template <typename T_>
struct remove_all_pointers<T_* const volatile> {
    using type = typename remove_all_pointers<T_>::type;
};
}

template <typename T_>
using remove_all_pointers_t = typename detail::remove_all_pointers<T_>::type;

namespace detail {
template <typename T_, bool IsConst_, bool IsVolatile_, bool IsLValueReference_, bool IsRValueReference_, bool IsPointer_>
struct TypeTraitsBase {
    using Type = T_;
    
    static constexpr bool IsConst = IsConst_;
    static constexpr bool IsVolatile = IsVolatile_;
    
    static constexpr bool IsLValueReference = IsLValueReference_;
    static constexpr bool IsRValueReference = IsRValueReference_;
    static constexpr bool IsReference = IsLValueReference_ || IsRValueReference_;

    static constexpr bool IsPointer = IsPointer_;
};
}

template <typename T_>
struct TypeTraits : detail::TypeTraitsBase<T_, false, false, false, false, false> {};

template <typename T_>
struct TypeTraits<const T_> : detail::TypeTraitsBase<T_, true, false, false, false, false> {};

template <typename T_>
struct TypeTraits<volatile T_> : detail::TypeTraitsBase<T_, false, true, false, false, false> {};

template <typename T_>
struct TypeTraits<const volatile T_> : detail::TypeTraitsBase<T_, true, true, false, false, false> {};

template <typename T_>
struct TypeTraits<T_&> : detail::TypeTraitsBase<T_, false, false, true, false, false> {};

template <typename T_>
struct TypeTraits<const T_&> : detail::TypeTraitsBase<T_, true, false, true, false, false> {};

template <typename T_>
struct TypeTraits<volatile T_&> : detail::TypeTraitsBase<T_, false, true, true, false, false> {};

template <typename T_>
struct TypeTraits<const volatile T_&> : detail::TypeTraitsBase<T_, true, true, true, false, false> {};

template <typename T_>
struct TypeTraits<T_&&> : detail::TypeTraitsBase<T_, false, false, false, true, false> {};

template <typename T_>
struct TypeTraits<const T_&&> : detail::TypeTraitsBase<T_, true, false, false, true, false> {};

template <typename T_>
struct TypeTraits<volatile T_&&> : detail::TypeTraitsBase<T_, false, true, false, true, false> {};

template <typename T_>
struct TypeTraits<const volatile T_&&> : detail::TypeTraitsBase<T_, true, true, false, true, false> {};

template <typename T_>
struct TypeTraits<T_*> : detail::TypeTraitsBase<T_, false, false, false, false, true> {};

template <typename T_>
struct TypeTraits<T_* const> : detail::TypeTraitsBase<T_, true, false, false, false, true> {};

template <typename T_>
struct TypeTraits<T_* volatile> : detail::TypeTraitsBase<T_, false, true, false, false, true> {};

template <typename T_>
struct TypeTraits<T_* const volatile> : detail::TypeTraitsBase<T_, true, true, false, false, true> {};
}
