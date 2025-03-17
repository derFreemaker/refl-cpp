#pragma once

#include "refl-cpp/refl-cpp.hpp" 

namespace kpop {
struct Test;
}

template <>
struct ReflCpp::ReflectData<kpop::Test> {
    static TypeData Create();
};
