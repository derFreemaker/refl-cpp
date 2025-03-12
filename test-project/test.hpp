#pragma once

#include "refl-cpp.generated/kpop/test.generated.hpp"

#include <iostream>

namespace kpop {
struct REFLECT() Test {
    void foo(const char* str, const char* test) {
        std::cout << str << test << std::endl;
    }
};
}