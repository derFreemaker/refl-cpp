#pragma once

#include <type_traits>

namespace ReflCpp {
namespace detail {
template <typename T>
struct make_const {
    using type = std::add_const_t<std::remove_const_t<T>>;
};

template <typename T>
struct make_const<T&> {
    using type = typename make_const<T>::type&;
};

template <typename T>
struct make_const<T&&> {
    using type = typename make_const<T>::type&&;
};
}

template <typename T>
using make_const_t = typename detail::make_const<T>::type;

namespace detail {
template <typename T>
struct add_const_to_pointer;

template <typename T>
struct add_const_to_pointer<T*> {
    using type = const T*;
};

template <typename T>
struct add_const_to_pointer<T* const> {
    using type = const T* const;
};

template <typename T>
struct add_const_to_pointer<T* volatile> {
    using type = const T* volatile;
};

template <typename T>
struct add_const_to_pointer<T* const volatile> {
    using type = const T* const volatile;
};
}

template <typename T>
    requires std::is_pointer_v<T>
struct add_const_to_pointer {
    using type = typename detail::add_const_to_pointer<T>::type;
};

template <typename T>
using add_const_to_pointer_t = typename add_const_to_pointer<T>::type;

namespace detail {
template <typename T>
struct remove_const_from_pointer;

template <typename T>
struct remove_const_from_pointer<T*> {
    using type = std::remove_const_t<T>*;
};

template <typename T>
struct remove_const_from_pointer<T* const> {
    using type = std::remove_const_t<T>* const;
};

template <typename T>
struct remove_const_from_pointer<T* volatile> {
    using type = std::remove_const_t<T>* volatile;
};

template <typename T>
struct remove_const_from_pointer<T* const volatile> {
    using type = std::remove_const_t<T>* const volatile;
};
}

template <typename T>
    requires std::is_pointer_v<T>
struct remove_const_from_pointer {
    using type = typename detail::remove_const_from_pointer<T>::type;
};

template <typename T>
using remove_const_from_pointer_t = typename remove_const_from_pointer<T>::type;

template <typename T>
using make_lvalue_reference_t = std::_Const_thru_ref<T>&;

template <typename T>
using make_rvalue_reference_t = std::_Const_thru_ref<T>&&;

namespace detail {
template <typename T>
struct remove_all_pointers {
    using type = T;
};

template <typename T>
struct remove_all_pointers<T*> {
    using type = typename remove_all_pointers<T>::type;
};

template <typename T>
struct remove_all_pointers<T* const> {
    using type = typename remove_all_pointers<T>::type;
};

template <typename T>
struct remove_all_pointers<T* volatile> {
    using type = typename remove_all_pointers<T>::type;
};

template <typename T>
struct remove_all_pointers<T* const volatile> {
    using type = typename remove_all_pointers<T>::type;
};
}

template <typename T>
using remove_all_pointers_t = typename detail::remove_all_pointers<T>::type;

namespace detail {
template <typename T, bool IsConst_, bool IsVolatile_, bool IsLValueReference_, bool IsRValueReference_, bool IsPointer_>
struct TypeTraitsBase {
    using Type = T;

    static constexpr bool IsConst = IsConst_;
    static constexpr bool IsVolatile = IsVolatile_;

    static constexpr bool IsLValueReference = IsLValueReference_;
    static constexpr bool IsRValueReference = IsRValueReference_;
    static constexpr bool IsReference = IsLValueReference_ || IsRValueReference_;

    static constexpr bool IsPointer = IsPointer_;
};
}

template <typename T>
struct TypeTraits : detail::TypeTraitsBase<T, false, false, false, false, false> {};

template <typename T>
struct TypeTraits<const T> : detail::TypeTraitsBase<T, true, false, false, false, false> {};

template <typename T>
struct TypeTraits<volatile T> : detail::TypeTraitsBase<T, false, true, false, false, false> {};

template <typename T>
struct TypeTraits<const volatile T> : detail::TypeTraitsBase<T, true, true, false, false, false> {};

template <typename T>
struct TypeTraits<T&> : detail::TypeTraitsBase<T, false, false, true, false, false> {};

template <typename T>
struct TypeTraits<const T&> : detail::TypeTraitsBase<T, true, false, true, false, false> {};

template <typename T>
struct TypeTraits<volatile T&> : detail::TypeTraitsBase<T, false, true, true, false, false> {};

template <typename T>
struct TypeTraits<const volatile T&> : detail::TypeTraitsBase<T, true, true, true, false, false> {};

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
