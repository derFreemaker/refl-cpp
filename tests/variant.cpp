#include <gtest/gtest.h>

#include "helper/variant_helper.hpp"

namespace ReflCpp {
struct TestStruct {
    int value = 42;
};

template <>
struct ReflectData<TestStruct> {
    static TypeData Create() {
        return TypeData{ .name = "TestStruct" };
    }
};

// Helper function to create variants for testing
template <typename T>
rescpp::result<Variant, ReflectError> CreateVariant(T value) {
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
    const auto variant = CreateVariant<int>(value).value();

    EXPECT_TRUE(variant.CanGet<int>());
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_TRUE(variant.CanGet<int&>());
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_EQ(variant.Get<int>().value(), 42);
    EXPECT_EQ(variant.Get<const int>().value(), 42);
    EXPECT_EQ(variant.Get<int&>().value(), 42);
    EXPECT_EQ(variant.Get<const int&>().value(), 42);

    // Modifying through reference should modify the original
    variant.Get<int&>().value() = 100;
    EXPECT_EQ(variant.Get<int>().value(), 100);
}

// Test const value handling
TEST(Variant, ConstValue) {
    constexpr int value = 42;
    const auto variant = CreateVariant<const int>(value).value();

    EXPECT_FALSE(variant.CanGet<int>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_FALSE(variant.CanGet<int&>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_EQ(variant.Get<const int>().value(), 42);
    EXPECT_EQ(variant.Get<const int&>().value(), 42);
}

// Test reference handling
TEST(Variant, LValueReference) {
    int value = 42;
    const auto variant = CreateVariant<int&>(value).value();

    EXPECT_TRUE(variant.CanGet<int>());
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_TRUE(variant.CanGet<int&>());
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_EQ(variant.Get<int>().value(), 42);
    EXPECT_EQ(variant.Get<const int>().value(), 42);
    EXPECT_EQ(variant.Get<int&>().value(), 42);
    EXPECT_EQ(variant.Get<const int&>().value(), 42);

    // Modifying through reference should modify the original
    variant.Get<int&>().value() = 100;
    EXPECT_EQ(value, 100);
    EXPECT_EQ(variant.Get<int>().value(), 100);
}

TEST(Variant, ConstLValueReference) {
    int value = 42;
    const auto variant = CreateVariant<const int&>(value).value();

    EXPECT_FALSE(variant.CanGet<int>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_FALSE(variant.CanGet<int&>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_EQ(variant.Get<const int>().value(), 42);
    EXPECT_EQ(variant.Get<const int&>().value(), 42);

    // Modifying the original should be visible through the variant
    // ReSharper disable once CppDFAUnusedValue
    value = 100;
    EXPECT_EQ(variant.Get<const int>().value(), 100);
}

TEST(Variant, RValueReference) {
    const auto v1 = CreateVariant<int&&>(42).value();

    EXPECT_TRUE(v1.CanGet<int&&>());
    EXPECT_TRUE(v1.CanGet<const int&&>());
    EXPECT_EQ(v1.Get<int&&>().value(), 42);

    const auto v2 = CreateVariant<const int&&>(42).value();
    EXPECT_FALSE(v2.CanGet<int&&>());
    EXPECT_TRUE(v2.CanGet<const int&&>());
    EXPECT_EQ(v2.Get<const int&&>().value(), 42);
}

TEST(Variant, ConstRValueReference) {
    int value = 42;
    const auto variant = CreateVariant<const int&&>(std::move(value)).value();

    // Test CanGet for rvalue reference
    EXPECT_TRUE(variant.CanGet<const int&&>());

    // Test actual Get for rvalue reference
    EXPECT_EQ(variant.Get<const int&&>().value(), 42);
}

TEST(Variant, Pointer) {
    int value = 42;
    const auto variant = CreateVariant<int*>(&value).value();

    EXPECT_TRUE(variant.CanGet<int*>());
    EXPECT_TRUE(variant.CanGet<const int*>());

    EXPECT_EQ(variant.Get<int*>().value(), &value);
    EXPECT_EQ(variant.Get<const int*>().value(), &value);

    // Modifying through pointer should modify the original
    *variant.Get<int*>().value() = 100;
    EXPECT_EQ(value, 100);
    EXPECT_EQ(variant.Get<int*>().value(), &value);
}

TEST(Variant, ConstPointer) {
    int value = 42;
    const auto variant = CreateVariant<const int*>(&value).value();

    EXPECT_FALSE(variant.CanGet<int*>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int*>());

    EXPECT_EQ(variant.Get<const int*>().value(), &value);

    // Modifying the original should be visible through the variant
    value = 100;
    EXPECT_EQ(*variant.Get<const int*>().value(), 100);
}

// Test with custom class
TEST(Variant, CustomClassGet) {
    TestStruct obj;
    obj.value = 42;
    const auto variant = CreateVariant<TestStruct>(obj).value();

    EXPECT_TRUE(variant.CanGet<TestStruct>());
    EXPECT_TRUE(variant.CanGet<const TestStruct>());
    EXPECT_TRUE(variant.CanGet<TestStruct&>());
    EXPECT_TRUE(variant.CanGet<const TestStruct&>());

    EXPECT_EQ(variant.Get<TestStruct>().value().value, 42);
    EXPECT_EQ(variant.Get<const TestStruct>().value().value, 42);
    EXPECT_EQ(variant.Get<TestStruct&>().value().value, 42);
    EXPECT_EQ(variant.Get<const TestStruct&>().value().value, 42);

    // Modifying through reference should modify the variant's copy
    variant.Get<TestStruct&>().value().value = 100;
    EXPECT_EQ(variant.Get<TestStruct>().value().value, 100);
}

// Test type mismatches
TEST(Variant, TypeMismatch) {
    constexpr int value = 42;
    const auto variant = CreateVariant<int>(value).value();

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
    const auto v1 = CreateVariant<TestStruct>(obj).value(); // value
    const auto v2 = CreateVariant<TestStruct&>(obj).value(); // reference
    const auto v3 = CreateVariant<TestStruct*>(&obj).value(); // pointer
    const auto v4 = CreateVariant<TestStruct&&>(static_cast<TestStruct&&>(obj)).value(); // rvalue

    // Check value copy
    EXPECT_TRUE(v1.CanGet<TestStruct>());
    EXPECT_EQ(v1.Get<TestStruct>().value().value, 42);

    // Check reference wrapper
    EXPECT_TRUE(v2.CanGet<TestStruct&>());
    EXPECT_EQ(v2.Get<TestStruct&>().value().value, 42);

    // Check pointer
    EXPECT_TRUE(v3.CanGet<TestStruct*>());
    EXPECT_EQ(v3.Get<TestStruct*>().value()->value, 42);

    // Check rvalue
    EXPECT_TRUE(v4.CanGet<TestStruct&&>());
    EXPECT_EQ(v4.Get<TestStruct&&>().value().value, 42);

    // Modify original and check pointer reflects change
    obj.value = 100;
    EXPECT_EQ(v2.Get<TestStruct&>().value().value, 100);
    EXPECT_EQ(v3.Get<TestStruct*>().value()->value, 100);
}
}
