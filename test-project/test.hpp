#pragma once

#include "refl-cpp.generated/kpop/test.generated.hpp"

#include <iostream>

namespace kpop {

struct REFLECT() Test {
    int test;

    void foo(const char* _foo) {
        std::cout << test << " str: " << _foo << std::endl;
    }

    void foo(int _foo) {
        std::cout << test << " int: " << _foo << std::endl;
    }
};
}
