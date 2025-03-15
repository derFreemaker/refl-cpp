#include <optional>

#include "refl-cpp/refl-cpp.hpp"

#include "test.hpp"

int main() {
    kpop::Test test;
    
    const auto field = ReflCpp::Reflect<kpop::Test>().GetField("test")->get();
    const auto method = ReflCpp::Reflect<kpop::Test>().GetMethod("foo")->get();
    
    field.SetValue(test, std::optional(123));
    
    const ReflCpp::Variant test_arg("foo");
    
    (void)method.Invoke(test, test_arg);
}
