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

template <>
struct ReflectData<NoCopyOrMoveStruct> {
    static TypeData Create() {
        return TypeData{ .name = "NoCopyOrMoveStruct" };
    }
};
}