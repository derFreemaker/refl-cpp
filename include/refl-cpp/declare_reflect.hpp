#pragma once

#include <vector>

#include "refl-cpp/type_id.hpp"

namespace ReflCpp {
struct Type;

template <typename T>
[[nodiscard]]
const Type& Reflect();

template <typename T>
[[nodiscard]]
TypeID ReflectID();

[[nodiscard]]
const Type& Reflect(TypeID id);

namespace detail {
template <typename Tuple_, std::size_t... I_>
static std::vector<TypeID> ReflectTupleTypes(std::index_sequence<I_...>) {
    return {ReflectID<std::tuple_element_t<I_, Tuple_>>()...};
}

template <typename Tuple_>
static std::vector<TypeID> ReflectTupleTypes() {
    return ReflectTupleTypes<Tuple_>(
        std::make_index_sequence<std::tuple_size_v<Tuple_>>());
}
}
}
