#pragma once

#include "refl-cpp.generated/kpop/test.generated.hpp"

#include <iostream>

namespace kpop {
struct REFLECT() Test {
    void foo(const char* str) {
        std::cout << str << ReflCpp::Reflect<Test>().GetMethod("foo")->GetArgument(0).name << std::endl;
    }
};
}