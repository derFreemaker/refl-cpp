#include <optional>
#include <print>

#include "refl-cpp/refl-cpp.hpp"

#include "refl-cpp/common/result.hpp"

#include "test.hpp"

#define EXIT_MAIN_TRY(...) \
    TRY_IMPL((__VA_ARGS__), { \
        printf("%s\n", __result__.Error().Str().c_str()); \
        return 1; \
    })

int main() {
    kpop::Test test{};

    const auto& type = EXIT_MAIN_TRY(ReflCpp::Reflect<kpop::Test>());

    const auto field = EXIT_MAIN_TRY(type.GetField("test"));
    const auto method = EXIT_MAIN_TRY(type.GetMethod("foo"));

    (void)EXIT_MAIN_TRY(method.Invoke(
        ReflCpp::Variant::Create<kpop::Test&>(test),
        {
        ReflCpp::Variant::Create<int>(123)
        }
    ));
    (void)EXIT_MAIN_TRY(method.Invoke(
        ReflCpp::Variant::Create<kpop::Test&>(test),
        {
        ReflCpp::Variant::Create<const char*>("test")
        }
    ));

    return 0;
}
