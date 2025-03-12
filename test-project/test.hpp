#pragma once

#include "refl-cpp.generated/kpop/test.generated.hpp"

#include <iostream>

namespace kpop {
struct REFLECT() Test {
    template <std::indirectly_writable T>
    void foo(T str, const char* test) {
        std::cout << str << test << std::endl;
    }
};
}