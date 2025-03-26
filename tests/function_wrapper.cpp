// #include <gtest/gtest.h>
//
// #include "helper/variant_helper.hpp"
// #include "refl-cpp/function_wrapper.hpp"
//
// namespace ReflCpp {
// struct NoCopyOrMoveStruct {
//     int foo = 893745;
//
//     NoCopyOrMoveStruct() = delete;
//     NoCopyOrMoveStruct(NoCopyOrMoveStruct&) = delete;
//     NoCopyOrMoveStruct(const NoCopyOrMoveStruct&) = delete;
//     NoCopyOrMoveStruct(NoCopyOrMoveStruct&&) = delete;
//     NoCopyOrMoveStruct(const NoCopyOrMoveStruct&&) = delete;
//
//     explicit NoCopyOrMoveStruct(std::nullptr_t) {}
// };
//
// template <>
// struct ReflectData<NoCopyOrMoveStruct> {
//     static Result<TypeData> Create() {
//         return TypeData { .name = "NoCopyOrMoveStruct" };
//     }
// };
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
// };
//
// template <>
// struct ReflectData<TestStruct> {
//     static Result<TypeData> Create() {
//         return TypeData { .name = "TestStruct" };
//     }
// };
//
// #define CREATE_METHOD_TEST_IMPL(NAME, INSTANCE, INSTANCE_TYPE, RETURN, ARG) \
//     TEST(FunctionWrapper, NAME) { \
//         const FunctionWrapper funcWrapper(&TestStruct::NAME); \
//         ARG NoCopyOrMoveStruct test(nullptr); \
//         INSTANCE_TYPE instance = INSTANCE; \
//         const auto result = funcWrapper.Invoke({ test }, instance).Value(); \
//         const auto result_value = result.GetValue<RETURN NoCopyOrMoveStruct&>(); \
//         ASSERT_EQ(result_value.Value().foo, 893745); \
//     }
//
// #define CREATE_METHOD_TEST(NAME, INSTANCE, INSTANCE_TYPE) \
//     CREATE_METHOD_TEST_IMPL(NAME, INSTANCE, INSTANCE_TYPE, , ) \
//     CREATE_METHOD_TEST_IMPL(NAME##_Const, INSTANCE, INSTANCE_TYPE, , const) \
//     CREATE_METHOD_TEST_IMPL(Const_##NAME, INSTANCE, INSTANCE_TYPE, const, ) \
//     CREATE_METHOD_TEST_IMPL(Const_##NAME##_Const, INSTANCE, INSTANCE_TYPE, const, const)
//
// #define CREATE_METHOD_TESTS(NAME, INSTANCE) \
//     CREATE_METHOD_TEST(NAME, INSTANCE, auto) \
//     CREATE_METHOD_TEST(NAME##Const, INSTANCE, const auto&)
//
// CREATE_METHOD_TEST(Static, Variant::Void(), auto)
//
// CREATE_METHOD_TESTS(Normal, TestStruct())
// CREATE_METHOD_TESTS(LValueReference, TestStruct())
// CREATE_METHOD_TESTS(RValueReference, TestStruct())
// }
