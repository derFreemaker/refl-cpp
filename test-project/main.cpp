#include <optional>

#include "refl-cpp/refl-cpp.hpp"

#include "test.hpp"

int main() {
    kpop::Test test;
    
    const auto field = *ReflCpp::Reflect<kpop::Test>().GetField("test");
    const auto method = *ReflCpp::Reflect<kpop::Test>().GetMethod("foo");
    
    ReflCpp::Variant test_value(123);
    
    field.SetValue(test, test_value);
    
    const ReflCpp::Variant test_arg("foo");
    
    (void)method.Invoke(test, test_arg);
}
