#include <optional>

#include "refl-cpp/refl-cpp.hpp"

#include "test.hpp"

int main() {
    kpop::Test test;
    
    const auto field = ReflCpp::Reflect<kpop::Test>().GetField("test")->get();
    const auto method = ReflCpp::Reflect<kpop::Test>().GetMethod("foo")->get();

    field.SetValue(123, test);
    const auto lol = field.GetValue<int>(test);
    printf("%i\n", lol);
    
    (void)method.Invoke(test, "foo");
}
