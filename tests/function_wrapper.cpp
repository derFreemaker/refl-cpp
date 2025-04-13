#include "config.hpp"
#include "helper/variant_helper.hpp"

namespace ReflCpp {
struct NoCopyOrMoveStruct {
    int foo = 893745;

    NoCopyOrMoveStruct() = delete;
    NoCopyOrMoveStruct(NoCopyOrMoveStruct&) = delete;
    NoCopyOrMoveStruct(const NoCopyOrMoveStruct&) = delete;
    NoCopyOrMoveStruct(NoCopyOrMoveStruct&&) = delete;
    NoCopyOrMoveStruct(const NoCopyOrMoveStruct&&) = delete;

    explicit NoCopyOrMoveStruct(std::nullptr_t) {}
};

template <>
struct ReflectData<NoCopyOrMoveStruct> {
    static Result<TypeData> Create() {
        return TypeData{ .name = "NoCopyOrMoveStruct" };
    }
};

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

template <>
struct ReflectData<TestStruct> {
    static Result<TypeData> Create() {
        return TypeData{ .name = "TestStruct" };
    }
};

#define CREATE_METHOD_TEST_IMPL(NAME, INSTANCE_TYPE, RETURN, ARG, ...) \
    TEST(FunctionWrapper, NAME) { \
        const FunctionWrapper funcWrapper(&TestStruct::NAME); \
        ARG NoCopyOrMoveStruct test(nullptr); \
        INSTANCE_TYPE __VA_ARGS__; \
        const auto result = FAIL_TRY(funcWrapper.Invoke({ FAIL_TRY(Variant::Create<ARG NoCopyOrMoveStruct&>(test)) }, instance)); \
        const auto& value = FAIL_TRY(result.Get<RETURN NoCopyOrMoveStruct&>()); \
        ASSERT_EQ(value.foo, 893745); \
    }

#define CREATE_METHOD_TEST(NAME, INSTANCE_TYPE, ...) \
    CREATE_METHOD_TEST_IMPL(NAME, INSTANCE_TYPE, , , __VA_ARGS__) \
    CREATE_METHOD_TEST_IMPL(NAME##_Const, INSTANCE_TYPE, , const, __VA_ARGS__) \
    CREATE_METHOD_TEST_IMPL(Const_##NAME, INSTANCE_TYPE, const, , __VA_ARGS__) \
    CREATE_METHOD_TEST_IMPL(Const_##NAME##_Const, INSTANCE_TYPE, const, const, __VA_ARGS__)

#define CREATE_METHOD_TESTS(NAME, ...) \
    CREATE_METHOD_TEST(NAME, auto, __VA_ARGS__) \
    CREATE_METHOD_TEST(NAME##Const, const auto&, __VA_ARGS__)

CREATE_METHOD_TEST(Static, auto, instance = Variant::Void())

CREATE_METHOD_TESTS(Normal, instance = FAIL_TRY(Variant::Create<TestStruct>(TestStruct())))
CREATE_METHOD_TESTS(LValueReference, instance = FAIL_TRY(Variant::Create<TestStruct&>(*new TestStruct())))
CREATE_METHOD_TESTS(RValueReference, instance = FAIL_TRY(Variant::Create<TestStruct&&>(TestStruct())))

TEST(FunctionWrapper, Pointer) {
    const FunctionWrapper funcWrapper(&TestStruct::Pointer);
    NoCopyOrMoveStruct test(nullptr);
    const auto instance = FAIL_TRY(Variant::Create<TestStruct>(TestStruct()));
    const auto result = FAIL_TRY(funcWrapper.Invoke({ FAIL_TRY(Variant::Create<NoCopyOrMoveStruct*>(&test)) }, instance));
    const auto& value = FAIL_TRY(result.Get<NoCopyOrMoveStruct&>());
    ASSERT_EQ(value.foo, 893745);
}

TEST(FunctionWrapper, Pointer_Const) {
    const FunctionWrapper funcWrapper(&TestStruct::Pointer_Const);
    const NoCopyOrMoveStruct test(nullptr);
    const auto instance = FAIL_TRY(Variant::Create<TestStruct>(TestStruct()));
    const auto result = FAIL_TRY(funcWrapper.Invoke({ FAIL_TRY(Variant::Create<const NoCopyOrMoveStruct*>(&test)) }, instance));
    const auto& value = FAIL_TRY(result.Get<NoCopyOrMoveStruct&>());
    ASSERT_EQ(value.foo, 893745);
}
}
