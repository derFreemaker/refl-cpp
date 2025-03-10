#pragma once

#include <vector>

#include "refl-cpp/argument.hpp"

namespace ReflCpp {
struct Function {
    const char* name;
    std::vector<Argument> arguments;
};
}
