#pragma once

#include <refl-cpp/refl-cpp.hpp>

namespace ReflCpp {
struct NoCopyOrMoveStruct {
    int foo = 893745;

    NoCopyOrMoveStruct() = delete;
    NoCopyOrMoveStruct(NoCopyOrMoveStruct&) = delete;
    NoCopyOrMoveStruct(const NoCopyOrMoveStruct&) = delete;
    NoCopyOrMoveStruct(NoCopyOrMoveStruct&&) = delete;
    NoCopyOrMoveStruct(const NoCopyOrMoveStruct&&) = delete;

    explicit NoCopyOrMoveStruct(std::nullptr_t) {}
};
}

REFLCPP_REFLECT_TEMPLATE()
REFLCPP_REFLECT_DATA(ReflCpp::NoCopyOrMoveStruct){
    .name = "NoCopyOrMoveStruct"
}
REFLCPP_REFLECT_DATA_END()
