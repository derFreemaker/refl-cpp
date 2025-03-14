#include <optional>

#include "refl-cpp/refl-cpp.hpp"

#include "test.hpp"

int main() {
    kpop::Test test;

    const auto field = *ReflCpp::Reflect<kpop::Test>().GetField("test");
    field.SetValue(test, 123);

    test.foo();
}
