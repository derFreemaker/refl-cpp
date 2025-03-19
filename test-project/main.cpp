#include <optional>

#include "refl-cpp/refl-cpp.hpp"

#include "refl-cpp/common/result.hpp"

#include "test.hpp"

jmp_buf jump_buffer;

ReflCpp::Result<std::string> foo() {
    return { ReflCpp::Error, "lol: {0}", 123 };
}

int main() {
    auto something = foo();
    
    // kpop::Test test;
    //
    // const auto field = ReflCpp::Reflect<kpop::Test>().GetField("test")->get();
    // const auto method = ReflCpp::Reflect<kpop::Test>().GetMethod("foo")->get();
    //
    // field.SetValue(std::optional(123), test);
    // // const auto lol = field.GetValue<std::optional<int>>(test);
    // // printf("%i\n", lol);
    //
    // (void)method.Invoke(test, "foo");
}
