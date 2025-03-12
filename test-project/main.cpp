#include <optional>
#include <iostream>

#include "refl-cpp/refl-cpp.hpp"

#include "test.hpp"

struct Test2 : kpop::Test {
    Test2(const std::optional<bool>& foo1)
        : foo(foo1) {}

    std::optional<bool> foo;
};

// namespace ReflCpp {
// template <>
// struct ReflectData<Test2> : ReflectTypeData {
//     static ReflectTypeData Create() {
//         return {
//             .name = "Test2",
//             .bases = {
//                 Reflect<kpop::Test>()
//             }
//         };
//     }
// };
// }
// template <>
// struct ReflCpp::ReflectPrinter<Test2> {
//     static void Print(std::ostream& stream, const Type& type) {
//         stream << "Test2 Print Override";
//     }
// };

int main() {
    kpop::Test test;

    auto name = ReflCpp::Reflect<kpop::Test>().GetMethod("foo")->GetArgument(0).name;
    ReflCpp::Variant arg(name);

    (void)ReflCpp::Reflect<kpop::Test>()
          .GetMethod("foo")->Invoke(
              test, {
                  "test",
                  "test"
              }
          );
}
