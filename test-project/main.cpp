#include <optional>
#include <print>

#include "refl-cpp/refl-cpp.hpp"

#include "refl-cpp/common/result.hpp"

#include "test.hpp"

// ReflCpp::Result<void> foo() {
//     return { RESULT_ERROR(), "lol: {0}", 123 };
// }

#define EXIT_MAIN_TRY(...) \
    CUSTOM_TRY((__VA_ARGS__), { \
        std::print("%s\n", __result__.Error().Str().c_str()); \
        return 1; \
    })

int main() {
    // auto something = foo();
    
    kpop::Test test{};

    const auto& type = EXIT_MAIN_TRY(ReflCpp::Reflect<kpop::Test>());

    const auto field = EXIT_MAIN_TRY(type.GetField("test"));
    const auto method = EXIT_MAIN_TRY(type.GetMethod("foo"));
    
    (void)EXIT_MAIN_TRY(method.Invoke(ReflCpp::Variant::Create<kpop::Test&>(test), {
        ReflCpp::Variant::Create<int>(123)
    }));
    (void)EXIT_MAIN_TRY(method.Invoke(ReflCpp::Variant::Create<kpop::Test&>(test), {
        ReflCpp::Variant::Create<const char*>("test")
    }));
    
    std::print("$END$");
}
