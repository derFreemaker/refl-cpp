#pragma once

#include <vector>

#include "refl-cpp/type_id.hpp"

namespace ReflCpp {
struct Type;

struct ArgumentInfo {
    const char* name;
    const TypeID type;
};

using ArgumentList = std::vector<Variant>;
}
