#include <array>
#include <cstdio>
#include <optional>
#include <iostream>

#include "refl-cpp/refl-cpp.hpp"
#include "refl-cpp.generated.hpp"

namespace kpop {
class REFLECT() Test {
public:
    void foo() {
        std::cout << "foo" << std::endl;
    }
};
}

struct REFLECT() Test2 : kpop::Test {
    Test2(const std::optional<bool>& foo1)
        : foo(foo1) {}

    std::optional<bool> foo;
};

template <>
struct ReflCpp::ReflectPrinter<Test2> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "Test2 Print Override";
    }
};

int main(int argc, char* argv[]) {
    const auto& info = ReflCpp::Reflect<std::optional<kpop::Test>>();
    const auto& info2 = ReflCpp::Reflect<std::optional<Test2*&>[]>();

    info.Print(std::cout);
    std::cout << "\n";

    ReflCpp::Reflect<char*[]>().Print(std::cout);
    std::cout << "\n";

    info2.Print(std::cout);
    std::cout << "\n";
}
