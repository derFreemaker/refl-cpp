#include <array>
#include <cstdio>
#include <optional>
#include <iostream>

#include "refl-cpp/refl-cpp.hpp"

namespace kpop {
class Test;
}

struct Test2;

namespace ReflCpp {
template <>
struct ReflectData<kpop::Test> : ReflectTypeData {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Test"
        };
    }
};

template <>
struct ReflectData<Test2> : ReflectTypeData {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Test2",
            .bases = {
                Reflect<kpop::Test>()
            }
        };
    }

    static void Print(std::ostream& stream, const Type& type) {
        stream << "Test2 Print Override";
    }
};
}

namespace kpop {
class REFLECT() Test {
public:
    explicit Test(int* foo)
        : foo(foo) {}

private:
    int* foo;
};
}

struct REFLECT() Test2 : kpop::Test {
    Test2(int* foo, const std::optional<bool>& foo1)
        : Test(foo),
          foo(foo1) {}

    std::optional<bool> foo;
};

int main(int argc, char* argv[]) {
    const auto& info = ReflCpp::Reflect<std::optional<Test2>>();
    const auto& info2 = ReflCpp::Reflect<std::optional<Test2*&>[]>();
    // const auto inner = info2.GetInner().GetInner();

    info2.Print(std::cout);

    // const auto is = info.Is(info2);
    // printf("%hhd %s\n", is, info2.GetName());
}
