#pragma once

#include "refl-cpp.generated/kpop/test.generated.hpp"

#include <iostream>

namespace kpop {
struct REFLECT() Test {
    int test;

    void foo(const char* _foo) {
        std::cout << test << " " << _foo << std::endl;
    }
};
}
