#pragma once

#include "refl-cpp.generated/kpop/test.generated.hpp"

#include <iostream>

namespace kpop {
struct REFLECT() Test {
    std::optional<int> test = 0;

    void foo(const char* _foo) {
        std::cout << *test << " " << _foo << std::endl;
    }
};
}
