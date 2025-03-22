#pragma once

#include <type_traits>

namespace ReflCpp {
namespace detail {
template <typename T_>
struct add_const_pointer_type<T_*> {
    using type = const T_*;
};

template <typename T_>
struct add_const_pointer_type<T_* const> {
    using type = const T_* const;
};

template <typename T_>
struct add_const_pointer_type<T_* volatile> {
    using type = const T_* volatile;
};

template <typename T_>
struct add_const_pointer_type<T_* const volatile> {
    using type = const T_* const volatile;
};
}

template <typename T_>
struct add_const_pointer_type {
    using type = typename detail::add_const_pointer_type<T_>::type;
};

template <typename T_>
using add_const_pointer_type_t = typename add_const_pointer_type<T_>::type;
}
