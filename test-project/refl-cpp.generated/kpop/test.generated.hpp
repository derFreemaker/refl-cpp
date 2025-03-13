#pragma once

#include "refl-cpp/refl-cpp.hpp" 

namespace kpop {
struct Test;
}

namespace ReflCpp {
template <>
struct ReflectData<kpop::Test> {
    static TypeData Create();
};
}