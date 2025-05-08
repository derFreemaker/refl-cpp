#include <optional>
#include <print>
#include <stacktrace>
#include <refl-cpp/refl-cpp.hpp>

#include "test.hpp"

#define EXIT_MAIN_TRY(...) \
    RESCPP_TRY_IMPL((__VA_ARGS__), { \
        printf("%s\n", std::to_string(std::stacktrace::current(0)).c_str()); \
        return 1; \
    })

int main() {
    kpop::Test test{};
    
    const auto& type = EXIT_MAIN_TRY(ReflCpp::Reflect<kpop::Test>());

    const auto field = type.GetField("test")->get();
    const auto method = type.GetMethod("foo")->get();

    (void)EXIT_MAIN_TRY(method.Invoke(
        EXIT_MAIN_TRY(ReflCpp::Variant::Create<kpop::Test&>(test)),
        {
        EXIT_MAIN_TRY(ReflCpp::Variant::Create<int>(123))
        }
    ));
    (void)EXIT_MAIN_TRY(method.Invoke(
        EXIT_MAIN_TRY(ReflCpp::Variant::Create<kpop::Test&>(test)),
        {
        EXIT_MAIN_TRY(ReflCpp::Variant::Create<const char*>("test"))
        }
    ));

    return 0;
}
