#include "config.hpp"
#include "helper/variant_helper.hpp"

namespace ReflCpp {
struct TestStruct {
    int value = 42;
};

template <>
struct ReflectData<TestStruct> {
    static Result<TypeData> Create() {
        return TypeData{ .name = "TestStruct" };
    }
};

// Helper function to create variants for testing
template <typename T>
Result<Variant> CreateVariant(T value) {
    return Variant::Create<T>(std::forward<T>(value));
}

// Test void variant
TEST(Variant, Void) {
    const auto& variant = Variant::Void();


    EXPECT_FALSE(variant.CanGet<int>());
    EXPECT_FALSE(variant.CanGet<const int>());
    EXPECT_FALSE(variant.CanGet<int&>());
    EXPECT_FALSE(variant.CanGet<const int&>());

    EXPECT_TRUE(variant.Get<int>().has_error());
    EXPECT_TRUE(variant.Get<const int>().has_error());
    EXPECT_TRUE(variant.Get<int&>().has_error());
    EXPECT_TRUE(variant.Get<const int&>().has_error());

    // Check that it is actually a void variant
    EXPECT_TRUE(variant.GetType().Equals<void>());
}

TEST(Variant, Value) {
    constexpr int value = 42;
    const auto variant = FAIL_TRY(CreateVariant<int>(value));

    EXPECT_TRUE(variant.CanGet<int>());
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_TRUE(variant.CanGet<int&>());
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_EQ(FAIL_TRY(variant.Get<int>()), 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<const int>()), 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<int&>()), 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<const int&>()), 42);

    // Modifying through reference should modify the original
    FAIL_TRY(variant.Get<int&>()) = 100;
    EXPECT_EQ(FAIL_TRY(variant.Get<int>()), 100);
}

// Test const value handling
TEST(Variant, ConstValue) {
    constexpr int value = 42;
    const auto variant = FAIL_TRY(CreateVariant<const int>(value));

    // Test CanGet
    EXPECT_FALSE(variant.CanGet<int>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_FALSE(variant.CanGet<int&>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int&>());

    // Test actual Get (only const should work)
    EXPECT_TRUE(variant.Get<int>().has_error());
    EXPECT_EQ(FAIL_TRY(variant.Get<const int>()), 42);
    EXPECT_TRUE(variant.Get<int&>().has_error());
    EXPECT_EQ(FAIL_TRY(variant.Get<const int&>()), 42);
}

// Test reference handling
TEST(Variant, LValueReference) {
    int value = 42;
    const auto variant = FAIL_TRY(CreateVariant<int&>(value));

    EXPECT_TRUE(variant.CanGet<int>());
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_TRUE(variant.CanGet<int&>());
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_EQ(FAIL_TRY(variant.Get<int>()), 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<const int>()), 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<int&>()), 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<const int&>()), 42);

    // Modifying through reference should modify the original
    FAIL_TRY(variant.Get<int&>()) = 100;
    EXPECT_EQ(value, 100);
    EXPECT_EQ(FAIL_TRY(variant.Get<int>()), 100);
}

TEST(Variant, ConstLValueReference) {
    int value = 42;
    const auto variant = FAIL_TRY(CreateVariant<const int&>(value));

    EXPECT_FALSE(variant.CanGet<int>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_FALSE(variant.CanGet<int&>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_TRUE(variant.Get<int>().has_error());
    EXPECT_EQ(FAIL_TRY(variant.Get<const int>()), 42);
    EXPECT_TRUE(variant.Get<int&>().has_error());
    EXPECT_EQ(FAIL_TRY(variant.Get<const int&>()), 42);

    // Modifying the original should be visible through the variant
    // ReSharper disable once CppDFAUnusedValue
    value = 100;
    EXPECT_EQ(FAIL_TRY(variant.Get<const int>()), 100);
}

TEST(Variant, RValueReference) {
    const auto variant = FAIL_TRY(CreateVariant<int&&>(42));

    EXPECT_TRUE(variant.CanGet<int&&>());
    EXPECT_TRUE(variant.CanGet<const int&&>());
    
    EXPECT_EQ(FAIL_TRY(variant.Get<int&&>()), 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<const int&&>()), 42);
}

TEST(Variant, ConstRValueReference) {
    int value = 42;
    const auto variant = FAIL_TRY(CreateVariant<const int&&>(std::move(value)));

    // Test CanGet for rvalue reference
    EXPECT_TRUE(variant.CanGet<const int&&>());

    // Test actual Get for rvalue reference
    EXPECT_EQ(FAIL_TRY(variant.Get<const int&&>()), 42);
}

TEST(Variant, Pointer) {
    int value = 42;
    const auto variant = FAIL_TRY(CreateVariant<int*>(&value));

    EXPECT_TRUE(variant.CanGet<int*>());
    EXPECT_TRUE(variant.CanGet<const int*>());

    EXPECT_EQ(*FAIL_TRY(variant.Get<int*>()), 42);
    EXPECT_EQ(*FAIL_TRY(variant.Get<const int*>()), 42);

    // Modifying through pointer should modify the original
    *FAIL_TRY(variant.Get<int*>()) = 100;
    EXPECT_EQ(value, 100);
    EXPECT_EQ(*FAIL_TRY(variant.Get<int*>()), 100);
}

TEST(Variant, ConstPointer) {
    int value = 42;
    const auto variant = FAIL_TRY(CreateVariant<const int*>(&value));

    EXPECT_FALSE(variant.CanGet<int*>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int*>());

    EXPECT_TRUE(variant.Get<int*>().has_error());
    EXPECT_EQ(*FAIL_TRY(variant.Get<const int*>()), 42);

    // Modifying the original should be visible through the variant
    value = 100;
    EXPECT_EQ(*FAIL_TRY(variant.Get<const int*>()), 100);
}

// Test with custom class
TEST(Variant, CustomClassGet) {
    TestStruct obj;
    obj.value = 42;
    const auto variant = FAIL_TRY(CreateVariant<TestStruct>(obj));

    EXPECT_TRUE(variant.CanGet<TestStruct>());
    EXPECT_TRUE(variant.CanGet<const TestStruct>());
    EXPECT_TRUE(variant.CanGet<TestStruct&>());
    EXPECT_TRUE(variant.CanGet<const TestStruct&>());

    EXPECT_EQ(FAIL_TRY(variant.Get<TestStruct>()).value, 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<const TestStruct>()).value, 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<TestStruct&>()).value, 42);
    EXPECT_EQ(FAIL_TRY(variant.Get<const TestStruct&>()).value, 42);

    // Modifying through reference should modify the variant's copy
    FAIL_TRY(variant.Get<TestStruct&>()).value = 100;
    EXPECT_EQ(FAIL_TRY(variant.Get<TestStruct>()).value, 100);
}

// Test type mismatches
TEST(Variant, TypeMismatch) {
    constexpr int value = 42;
    const auto variant = FAIL_TRY(CreateVariant<int>(value));

    // Test CanGet with wrong types
    EXPECT_FALSE(variant.CanGet<double>());
    EXPECT_FALSE(variant.CanGet<std::string>());
    EXPECT_FALSE(variant.CanGet<TestStruct>());

    // Test actual Get with wrong types
    EXPECT_TRUE(variant.Get<double>().has_error());
    EXPECT_TRUE(variant.Get<std::string>().has_error());
    EXPECT_TRUE(variant.Get<TestStruct>().has_error());
}

// Test complex type conversions to ensure unified implementation works
TEST(Variant, ComplexTypeConversions) {
    TestStruct obj;
    obj.value = 42;

    // Create variants with different reference/pointer types
    const auto v1 = FAIL_TRY(CreateVariant<TestStruct>(obj)); // value
    const auto v2 = FAIL_TRY(CreateVariant<TestStruct&>(obj)); // reference
    const auto v3 = FAIL_TRY(CreateVariant<TestStruct*>(&obj)); // pointer
    const auto v4 = FAIL_TRY(CreateVariant<TestStruct&&>(static_cast<TestStruct&&>(obj))); // rvalue

    // Check value copy
    EXPECT_TRUE(v1.CanGet<TestStruct>());
    EXPECT_EQ(FAIL_TRY(v1.Get<TestStruct>()).value, 42);

    // Check reference wrapper
    EXPECT_TRUE(v2.CanGet<TestStruct&>());
    EXPECT_EQ(FAIL_TRY(v2.Get<TestStruct&>()).value, 42);

    // Check pointer
    EXPECT_TRUE(v3.CanGet<TestStruct*>());
    EXPECT_EQ(FAIL_TRY(v3.Get<TestStruct*>())->value, 42);

    // Check rvalue
    EXPECT_TRUE(v4.CanGet<TestStruct&&>());
    EXPECT_EQ(FAIL_TRY(v4.Get<TestStruct&&>()).value, 42);

    // Modify original and check pointer reflects change
    obj.value = 100;
    EXPECT_EQ(FAIL_TRY(v2.Get<TestStruct&>()).value, 100);
    EXPECT_EQ(FAIL_TRY(v3.Get<TestStruct*>())->value, 100);
}
}
