#pragma once

#include <iostream>
#include <refl-cpp/refl-cpp.hpp>

namespace kpop {
struct Test {
    int test;

    void foo(const char* _foo) {
        std::cout << test << " str: " << _foo << std::endl;
    }

    void foo(int _foo) {
        std::cout << test << " int: " << _foo << std::endl;
    }
};
}

#include "refl-cpp.generated/kpop/test.generated.reflection"
