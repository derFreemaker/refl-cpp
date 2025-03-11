#pragma once

#include <stdexcept>
#include <vector>

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/variant.hpp"

namespace ReflCpp {
struct Type;

struct ArgumentInfo {
    const char* name;
    const TypeID type;
};

typedef std::vector<Variant> ArgumentList;

}
