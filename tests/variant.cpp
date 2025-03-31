#include <gtest/gtest.h>

#include "refl-cpp/common/result.hpp"
#include "helper/variant_helper.hpp"

namespace ReflCpp {
struct TestStruct {
    int value = 42;
};

template <>
struct ReflectData<TestStruct> {
    static Result<TypeData> Create() {
        return TypeData { .name = "TestStruct" };
    }
};

// Helper function to create variants for testing
template <typename T>
Variant CreateVariant(T value) {
    return Variant::Create<T>(std::forward<T>(value));
}

TEST(Variant, BasicValueGet) {
    constexpr int value = 42;
    const auto variant = CreateVariant<int>(value);

    EXPECT_TRUE(variant.CanGet<int>());
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_TRUE(variant.CanGet<int&>());
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_EQ(variant.Get<int>().Value(), 42);
    EXPECT_EQ(variant.Get<const int>().Value(), 42);
    EXPECT_EQ(variant.Get<int&>().Value(), 42);
    EXPECT_EQ(variant.Get<const int&>().Value(), 42);

    // Modifying through reference should modify the original
    variant.Get<int&>().Value() = 100;
    EXPECT_EQ(variant.Get<int>().Value(), 100);
}

// Test const value handling
TEST(Variant, ConstValueGet) {
    constexpr int value = 42;
    const auto variant = CreateVariant<const int>(value);

    // Test CanGet
    EXPECT_FALSE(variant.CanGet<int>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_FALSE(variant.CanGet<int&>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int&>());

    // Test actual Get (only const should work)
    EXPECT_TRUE(variant.Get<int>().HasError());
    EXPECT_EQ(variant.Get<const int>().Value(), 42);
    EXPECT_TRUE(variant.Get<int&>().HasError());
    EXPECT_EQ(variant.Get<const int&>().Value(), 42);
}

// Test reference handling
TEST(Variant, ReferenceGet) {
    int value = 42;
    const auto variant = CreateVariant<int&>(value);

    EXPECT_TRUE(variant.CanGet<int>());
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_TRUE(variant.CanGet<int&>());
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_EQ(variant.Get<int>().Value(), 42);
    EXPECT_EQ(variant.Get<const int>().Value(), 42);
    EXPECT_EQ(variant.Get<int&>().Value(), 42);
    EXPECT_EQ(variant.Get<const int&>().Value(), 42);

    // Modifying through reference should modify the original
    variant.Get<int&>().Value() = 100;
    EXPECT_EQ(value, 100);
    EXPECT_EQ(variant.Get<int>().Value(), 100);
}

// Test const reference handling
TEST(Variant, ConstReferenceGet) {
    int value = 42;
    const auto variant = CreateVariant<const int&>(value);

    EXPECT_FALSE(variant.CanGet<int>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_FALSE(variant.CanGet<int&>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_TRUE(variant.Get<int>().HasError());
    EXPECT_EQ(variant.Get<const int>().Value(), 42);
    EXPECT_TRUE(variant.Get<int&>().HasError());
    EXPECT_EQ(variant.Get<const int&>().Value(), 42);

    // Modifying the original should be visible through the variant
    value = 100;
    EXPECT_EQ(variant.Get<const int>().Value(), 100);
}

// Test pointer handling
TEST(Variant, PointerGet) {
    int value = 42;
    const auto variant = CreateVariant<int*>(&value);

    EXPECT_TRUE(variant.CanGet<int*>());
    EXPECT_TRUE(variant.CanGet<const int*>());

    EXPECT_EQ(*variant.Get<int*>().Value(), 42);
    EXPECT_EQ(*variant.Get<const int*>().Value(), 42);

    // Modifying through pointer should modify the original
    *variant.Get<int*>().Value() = 100;
    EXPECT_EQ(value, 100);
    EXPECT_EQ(*variant.Get<int*>().Value(), 100);
}

// Test const pointer handling
TEST(Variant, ConstPointerGet) {
    int value = 42;
    auto variant = CreateVariant<const int*>(&value);

    EXPECT_FALSE(variant.CanGet<int*>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int*>());

    EXPECT_TRUE(variant.Get<int*>().HasError());
    EXPECT_EQ(*variant.Get<const int*>().Value(), 42);

    // Modifying the original should be visible through the variant
    value = 100;
    EXPECT_EQ(*variant.Get<const int*>().Value(), 100);
}

// Test rvalue handling
TEST(Variant, RValueGet) {
    auto variant = CreateVariant<int&&>(42);

    EXPECT_FALSE(variant.CanGet<int>());
    EXPECT_FALSE(variant.CanGet<const int>());
    EXPECT_FALSE(variant.CanGet<int&>());
    EXPECT_FALSE(variant.CanGet<const int&>());

    EXPECT_TRUE(variant.Get<int>().HasError());
    EXPECT_TRUE(variant.Get<const int>().HasError());
    EXPECT_TRUE(variant.Get<int&>().HasError());
    EXPECT_TRUE(variant.Get<const int&>().HasError());
}

// Test with custom class
TEST(Variant, CustomClassGet) {
    TestStruct obj;
    obj.value = 42;
    const auto variant = CreateVariant<TestStruct>(obj);

    EXPECT_TRUE(variant.CanGet<TestStruct>());
    EXPECT_TRUE(variant.CanGet<const TestStruct>());
    EXPECT_TRUE(variant.CanGet<TestStruct&>());
    EXPECT_TRUE(variant.CanGet<const TestStruct&>());

    EXPECT_EQ(variant.Get<TestStruct>().Value().value, 42);
    EXPECT_EQ(variant.Get<const TestStruct>().Value().value, 42);
    EXPECT_EQ(variant.Get<TestStruct&>().Value().value, 42);
    EXPECT_EQ(variant.Get<const TestStruct&>().Value().value, 42);

    // Modifying through reference should modify the variant's copy
    variant.Get<TestStruct&>().Value().value = 100;
    EXPECT_EQ(variant.Get<TestStruct>().Value().value, 100);
}

// Test void variant
TEST(Variant, VoidVariant) {
    const auto& variant = ::ReflCpp::Variant::Void();

    EXPECT_FALSE(variant.CanGet<int>());
    EXPECT_FALSE(variant.CanGet<const int>());
    EXPECT_FALSE(variant.CanGet<int&>());
    EXPECT_FALSE(variant.CanGet<const int&>());

    EXPECT_TRUE(variant.Get<int>().HasError());
    EXPECT_TRUE(variant.Get<const int>().HasError());
    EXPECT_TRUE(variant.Get<int&>().HasError());
    EXPECT_TRUE(variant.Get<const int&>().HasError());

    // Check that it is actually a void variant
    EXPECT_TRUE(variant.GetType().Equals<void>());
}

// Test type mismatches
TEST(Variant, TypeMismatch) {
    constexpr int value = 42;
    auto variant = CreateVariant<int>(value);

    // Test CanGet with wrong types
    EXPECT_FALSE(variant.CanGet<double>());
    EXPECT_FALSE(variant.CanGet<std::string>());
    EXPECT_FALSE(variant.CanGet<TestStruct>());

    // Test actual Get with wrong types
    EXPECT_TRUE(variant.Get<double>().HasError());
    EXPECT_TRUE(variant.Get<std::string>().HasError());
    EXPECT_TRUE(variant.Get<TestStruct>().HasError());
}

// Test rvalue reference specifically
TEST(Variant, RValueReference) {
    int value = 42;
    const auto variant = CreateVariant(std::move(value));

    // Test CanGet for rvalue reference
    EXPECT_TRUE(variant.CanGet<int&&>());

    // Test actual Get for rvalue reference
    EXPECT_EQ(variant.Get<int&&>().Value(), 42);
}

// Test complex type conversions to ensure unified implementation works
TEST(Variant, ComplexTypeConversions) {
    TestStruct obj;
    obj.value = 42;

    // Create variants with different reference/pointer types
    const auto v1 = CreateVariant<TestStruct>(obj); // value
    const auto v2 = CreateVariant<TestStruct&>(obj); // reference
    const auto v3 = CreateVariant<TestStruct*>(&obj); // pointer
    const auto v4 = CreateVariant<TestStruct&&>(std::move(obj)); // rvalue

    // Check value copy
    EXPECT_TRUE(v1.CanGet<TestStruct>());
    EXPECT_EQ(v1.Get<TestStruct>().Value().value, 42);

    // Check reference wrapper
    EXPECT_TRUE(v2.CanGet<TestStruct&>());
    EXPECT_EQ(v2.Get<TestStruct&>().Value().value, 42);

    // Check pointer
    EXPECT_TRUE(v3.CanGet<TestStruct*>());
    EXPECT_EQ(v3.Get<TestStruct*>().Value()->value, 42);

    // Check rvalue
    EXPECT_TRUE(v4.CanGet<TestStruct&&>());
    EXPECT_EQ(v4.Get<TestStruct&&>().Value().value, 42);

    // Modify original and check pointer reflects change
    obj.value = 100;
    EXPECT_EQ(v2.Get<TestStruct&>().Value().value, 100);
    EXPECT_EQ(v3.Get<TestStruct*>().Value()->value, 100);
}
}
