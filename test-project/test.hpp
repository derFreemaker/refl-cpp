#pragma once

#include "refl-cpp.generated/kpop/test.generated.hpp"

#include <iostream>

namespace kpop {
struct REFLECT() Test {
    int test = 0;

    void foo() {
        std::cout << test << std::endl;
    }
};
}
