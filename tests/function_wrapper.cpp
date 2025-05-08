// #include <gtest/gtest.h>
//
// #include "helper/variant_helper.hpp"
//
// #include "no_copy_or_move_struct.hpp"
//
// namespace ReflCpp {
//
// struct TestStruct {
// #define CREATE_TEST_METHOD(BEFORE, NAME, ARGS, AFTER) \
//     BEFORE NoCopyOrMoveStruct& NAME(ARGS NoCopyOrMoveStruct& foo) AFTER { \
//         return const_cast<NoCopyOrMoveStruct&>(foo); \
//     }
//
// #define CREATE_TEST_METHODS_IMPL(BEFORE, NAME, AFTER) \
//     CREATE_TEST_METHOD(BEFORE, NAME, , AFTER) \
//     CREATE_TEST_METHOD(BEFORE, NAME##_Const, const, AFTER) \
//     CREATE_TEST_METHOD(BEFORE const, Const_##NAME, , AFTER) \
//     CREATE_TEST_METHOD(BEFORE const, Const_##NAME##_Const, const, AFTER)
//
// #define CREATE_TEST_METHODS(NAME, AFTER) \
//     CREATE_TEST_METHODS_IMPL(, NAME, AFTER) \
//     CREATE_TEST_METHODS_IMPL(, NAME##Const, const AFTER)
//
//     CREATE_TEST_METHODS_IMPL(static, Static,)
//
//     CREATE_TEST_METHODS(Normal,)
//     CREATE_TEST_METHODS(LValueReference, &)
//     CREATE_TEST_METHODS(RValueReference, &&)
//
//     NoCopyOrMoveStruct& Pointer(NoCopyOrMoveStruct* foo) {
//         return const_cast<NoCopyOrMoveStruct&>(*foo);
//     }
//
//     NoCopyOrMoveStruct& Pointer_Const(const NoCopyOrMoveStruct* foo) {
//         return const_cast<NoCopyOrMoveStruct&>(*foo);
//     }
// };
//
// template <>
// struct ReflectData<TestStruct> {
//     static TypeData Create() {
//         return TypeData{ .name = "TestStruct" };
//     }
// };
//
// namespace testing {
// #define CREATE_METHOD_TEST_IMPL(NAME, INSTANCE_TYPE, RETURN, ARG, ...) \
//     TEST(FunctionWrapper, NAME) { \
//         const FunctionWrapper funcWrapper(&TestStruct::NAME); \
//         ARG NoCopyOrMoveStruct test(nullptr); \
//         INSTANCE_TYPE __VA_ARGS__; \
//         const auto result = funcWrapper.Invoke({ Variant::Create<ARG NoCopyOrMoveStruct&>(test).value() }, instance).value(); \
//         const auto& value = result.Get<RETURN NoCopyOrMoveStruct&>().value(); \
//         ASSERT_EQ(value.foo, 893745); \
//     }
//
// #define CREATE_METHOD_TEST(NAME, INSTANCE_TYPE, ...) \
//     CREATE_METHOD_TEST_IMPL(NAME, INSTANCE_TYPE, , , __VA_ARGS__) \
//     CREATE_METHOD_TEST_IMPL(NAME##_Const, INSTANCE_TYPE, , const, __VA_ARGS__) \
//     CREATE_METHOD_TEST_IMPL(Const_##NAME, INSTANCE_TYPE, const, , __VA_ARGS__) \
//     CREATE_METHOD_TEST_IMPL(Const_##NAME##_Const, INSTANCE_TYPE, const, const, __VA_ARGS__)
//
// #define CREATE_METHOD_TESTS(NAME, ...) \
//     CREATE_METHOD_TEST(NAME, auto, __VA_ARGS__) \
//     CREATE_METHOD_TEST(NAME##Const, const auto, __VA_ARGS__)
//
// CREATE_METHOD_TEST(Static, auto, instance = Variant::Void())
//
// CREATE_METHOD_TESTS(Normal, instance = Variant::Create<TestStruct>(TestStruct()).value())
// CREATE_METHOD_TESTS(LValueReference, instance = Variant::Create<TestStruct&>(*new TestStruct()).value())
// CREATE_METHOD_TESTS(RValueReference, instance = Variant::Create<TestStruct&&>(TestStruct()).value())
//
// TEST(FunctionWrapper, Pointer) {
//     const FunctionWrapper funcWrapper(&TestStruct::Pointer);
//     NoCopyOrMoveStruct test(nullptr);
//     const auto instance = Variant::Create<TestStruct>(TestStruct()).value();
//     const auto result = funcWrapper.Invoke({ Variant::Create<NoCopyOrMoveStruct*>(&test).value() }, instance).value();
//     const auto& value = result.Get<NoCopyOrMoveStruct&>().value();
//     ASSERT_EQ(value.foo, 893745);
// }
//
// TEST(FunctionWrapper, Pointer_Const) {
//     const FunctionWrapper funcWrapper(&TestStruct::Pointer_Const);
//     const NoCopyOrMoveStruct test(nullptr);
//     const auto instance = Variant::Create<TestStruct>(TestStruct()).value();
//     const auto result = funcWrapper.Invoke({ Variant::Create<const NoCopyOrMoveStruct*>(&test).value() }, instance).value();
//     const auto& value = result.Get<NoCopyOrMoveStruct&>().value();
//     ASSERT_EQ(value.foo, 893745);
// }
// }
// }

#include <catch2/catch_all.hpp>

#include "helper/variant_helper.hpp"
#include "no_copy_or_move_struct.hpp"

namespace ReflCpp {
struct TestStruct {
#define CREATE_TEST_METHOD(BEFORE, NAME, ARGS, AFTER) \
BEFORE NoCopyOrMoveStruct& NAME(ARGS NoCopyOrMoveStruct& foo) AFTER { \
return const_cast<NoCopyOrMoveStruct&>(foo); \
}

#define CREATE_TEST_METHODS_IMPL(BEFORE, NAME, AFTER) \
CREATE_TEST_METHOD(BEFORE, NAME, , AFTER) \
CREATE_TEST_METHOD(BEFORE, NAME##_Const, const, AFTER) \
CREATE_TEST_METHOD(BEFORE const, Const_##NAME, , AFTER) \
CREATE_TEST_METHOD(BEFORE const, Const_##NAME##_Const, const, AFTER)

#define CREATE_TEST_METHODS(NAME, AFTER) \
CREATE_TEST_METHODS_IMPL(, NAME, AFTER) \
CREATE_TEST_METHODS_IMPL(, NAME##Const, const AFTER)

    CREATE_TEST_METHODS_IMPL(static, Static,)

    CREATE_TEST_METHODS(Normal,)
    CREATE_TEST_METHODS(LValueReference, &)
    CREATE_TEST_METHODS(RValueReference, &&)

    NoCopyOrMoveStruct& Pointer(NoCopyOrMoveStruct* foo) {
        return const_cast<NoCopyOrMoveStruct&>(*foo);
    }

    NoCopyOrMoveStruct& Pointer_Const(const NoCopyOrMoveStruct* foo) {
        return const_cast<NoCopyOrMoveStruct&>(*foo);
    }
};
}

REFLCPP_REFLECT_TEMPLATE()
REFLCPP_REFLECT_DATA(ReflCpp::TestStruct){
        .name = "TestStruct",
        ._namespace = "ReflCpp"
    }
REFLCPP_REFLECT_DATA_END()

namespace ReflCpp::testing {
#define CREATE_METHOD_TEST_IMPL(NAME, INSTANCE_TYPE, RETURN, ARG, ...) \
    SECTION(#NAME, "[FunctionWrapper]") { \
        const FunctionWrapper funcWrapper(&TestStruct::NAME); \
        ARG NoCopyOrMoveStruct test(nullptr); \
        INSTANCE_TYPE __VA_ARGS__; \
        const auto result = funcWrapper.Invoke({ Variant::Create<ARG NoCopyOrMoveStruct&>(test).value() }, instance).value(); \
        const auto& value = result.Get<RETURN NoCopyOrMoveStruct&>().value(); \
        REQUIRE(value.foo == 893745); \
    }

#define CREATE_METHOD_TEST(NAME, INSTANCE_TYPE, ...) \
    CREATE_METHOD_TEST_IMPL(NAME, INSTANCE_TYPE, , , __VA_ARGS__) \
    CREATE_METHOD_TEST_IMPL(NAME##_Const, INSTANCE_TYPE, , const, __VA_ARGS__) \
    CREATE_METHOD_TEST_IMPL(Const_##NAME, INSTANCE_TYPE, const, , __VA_ARGS__) \
    CREATE_METHOD_TEST_IMPL(Const_##NAME##_Const, INSTANCE_TYPE, const, const, __VA_ARGS__)

#define CREATE_METHOD_TESTS(NAME, ...) \
    CREATE_METHOD_TEST(NAME, auto, __VA_ARGS__) \
    CREATE_METHOD_TEST(NAME##Const, const auto, __VA_ARGS__)

TEST_CASE("FunctionWrapper") {
    CREATE_METHOD_TEST(Static, auto, instance = Variant::Void())

    CREATE_METHOD_TESTS(Normal, instance = Variant::Create<TestStruct>(TestStruct()).value())
    CREATE_METHOD_TESTS(LValueReference, instance = Variant::Create<TestStruct&>(*new TestStruct()).value())
    CREATE_METHOD_TESTS(RValueReference, instance = Variant::Create<TestStruct&&>(TestStruct()).value())

    SECTION("Pointer", "[FunctionWrapper]") {
        const FunctionWrapper funcWrapper(&TestStruct::Pointer);
        NoCopyOrMoveStruct test(nullptr);
        const auto instance = Variant::Create<TestStruct>(TestStruct()).value();
        const auto result = funcWrapper.Invoke({ Variant::Create<NoCopyOrMoveStruct*>(&test).value() }, instance).value();
        const auto& value = result.Get<NoCopyOrMoveStruct&>().value();
        REQUIRE(value.foo == 893745);
    }

    SECTION("Pointer_Const", "[FunctionWrapper]") {
        const FunctionWrapper funcWrapper(&TestStruct::Pointer_Const);
        const NoCopyOrMoveStruct test(nullptr);
        const auto instance = Variant::Create<TestStruct>(TestStruct()).value();
        const auto result = funcWrapper.Invoke({ Variant::Create<const NoCopyOrMoveStruct*>(&test).value() }, instance).value();
        const auto& value = result.Get<NoCopyOrMoveStruct&>().value();
        REQUIRE(value.foo == 893745);
    }
}
} // namespace ReflCpp::testing
