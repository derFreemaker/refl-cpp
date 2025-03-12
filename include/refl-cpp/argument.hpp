#pragma once

#include <vector>

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/variant.hpp"

namespace ReflCpp {
struct Type;

struct ArgumentInfo {
    const char* name;
    const TypeID type;
};

struct ArgumentList {
    std::vector<Variant> data;

    template <typename... Args>
    ArgumentList(Args&&... args)
        : data({Variant(std::forward<Args>(args))...}) {}
};
}
