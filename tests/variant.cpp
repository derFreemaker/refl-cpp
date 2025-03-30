#include <gtest/gtest.h>

#include "refl-cpp/common/result.hpp"
#include "helper/variant_helper.hpp"

namespace ReflCpp {
//TODO: rewrite tests

TEST(Variant, Void) {
    const auto& variant = Variant::Void();

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<detail::VoidVariantWrapper>(variant));

    const auto getValueResult = variant.Get<int>();
    const auto getConstValueResult = variant.Get<const int>();
    const auto getLValueRefResult = variant.Get<int&>();
    const auto getConstLValueRefResult = variant.Get<const int&>();
    const auto getRValueRefResult = variant.Get<int&&>();
    const auto getConstRValueRefResult = variant.Get<const int&&>();
    const auto getPointerResult = variant.Get<int*>();
    const auto getConstPointerResult = variant.Get<const int*>();

    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getValueResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getConstLValueRefResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getConstRValueRefResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get reference or value from void variant");
}

TEST(Variant, Value) {
    const auto& variant = Variant::Create<int>(123);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<detail::ValueVariantWrapper<int>>(variant));

    const auto getValueResult = variant.Get<int>();
    const auto getConstValueResult = variant.Get<const int>();
    const auto getLValueRefResult = variant.Get<int&>();
    const auto getConstLValueRefResult = variant.Get<const int&>();
    const auto getRValueRefResult = variant.Get<int&&>();
    const auto getConstRValueRefResult = variant.Get<const int&&>();
    const auto getPointerResult = variant.Get<int*>();
    const auto getConstPointerResult = variant.Get<const int*>();

    ASSERT_EQ(getValueResult.Value(), 123);
    ASSERT_EQ(getConstValueResult.Value(), 123);
    ASSERT_EQ(getLValueRefResult.Value(), 123);
    ASSERT_EQ(getConstLValueRefResult.Value(), 123);
    ASSERT_EQ(getRValueRefResult.Value(), 123);
    ASSERT_EQ(getConstRValueRefResult.Value(), 123);
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (int32_t) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (int32_t) with passed type: const int32_t*");
}

TEST(Variant, Const_Value) {
    const auto& variant = Variant::Create<const int>(123);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<detail::ConstValueVariantWrapper<int>>(variant));

    const auto getValueResult = variant.Get<int>();
    const auto getConstValueResult = variant.Get<const int>();
    const auto getLValueRefResult = variant.Get<int&>();
    const auto getConstLValueRefResult = variant.Get<const int&>();
    const auto getRValueRefResult = variant.Get<int&&>();
    const auto getConstRValueRefResult = variant.Get<const int&&>();
    const auto getPointerResult = variant.Get<int*>();
    const auto getConstPointerResult = variant.Get<const int*>();

    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Value(), 123);
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: int32_t&");
    ASSERT_EQ(getConstLValueRefResult.Value(), 123);
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: int32_t&&");
    ASSERT_EQ(getConstRValueRefResult.Value(), 123);
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: const int32_t*");
}

TEST(Variant, LValueRef) {
    int value = 123;
    const auto& variant = Variant::Create<int&>(value);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<detail::LValueRefVariantWrapper<int>>(variant));

    const auto getValueResult = variant.Get<int>();
    const auto getConstValueResult = variant.Get<const int>();
    const auto getLValueRefResult = variant.Get<int&>();
    const auto getConstLValueRefResult = variant.Get<const int&>();
    const auto getRValueRefResult = variant.Get<int&&>();
    const auto getConstRValueRefResult = variant.Get<const int&&>();
    const auto getPointerResult = variant.Get<int*>();
    const auto getConstPointerResult = variant.Get<const int*>();

    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: const int32_t");
    ASSERT_EQ(getLValueRefResult.Value(), 123);
    ASSERT_EQ(getConstLValueRefResult.Value(), 123);
    ASSERT_EQ(getRValueRefResult.Value(), 123);
    ASSERT_EQ(getConstRValueRefResult.Value(), 123);
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: const int32_t*");
}

TEST(Variant, Const_LValueRef) {
    const auto& variant = Variant::Create<const int&>(123);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<detail::ConstLValueRefVariantWrapper<int>>(variant));

    const auto getValueResult = variant.Get<int>();
    const auto getConstValueResult = variant.Get<const int>();
    const auto getLValueRefResult = variant.Get<int&>();
    const auto getConstLValueRefResult = variant.Get<const int&>();
    const auto getRValueRefResult = variant.Get<int&&>();
    const auto getConstRValueRefResult = variant.Get<const int&&>();
    const auto getPointerResult = variant.Get<int*>();
    const auto getConstPointerResult = variant.Get<const int*>();

    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: const int32_t");
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: int32_t&");
    ASSERT_EQ(getConstLValueRefResult.Value(), 123);
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: int32_t&&");
    ASSERT_EQ(getConstRValueRefResult.Value(), 123);
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: const int32_t*");
}

TEST(Variant, RValueRef) {
    const auto& variant = Variant::Create<int&&>(123);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<detail::RValueRefVariantWrapper<int>>(variant));

    const auto getValueResult = variant.Get<int>();
    const auto getConstValueResult = variant.Get<const int>();
    const auto getLValueRefResult = variant.Get<int&>();
    const auto getConstLValueRefResult = variant.Get<const int&>();
    const auto getRValueRefResult = variant.Get<int&&>();
    const auto getConstRValueRefResult = variant.Get<const int&&>();
    const auto getPointerResult = variant.Get<int*>();
    const auto getConstPointerResult = variant.Get<const int*>();

    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (int32_t&&) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get from Variant (int32_t&&) with passed type: const int32_t");
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get from Variant (int32_t&&) with passed type: int32_t&");
    ASSERT_EQ(getConstLValueRefResult.Error().Message(), "cannot get from Variant (int32_t&&) with passed type: const int32_t&");
    ASSERT_EQ(getRValueRefResult.Value(), 123);
    ASSERT_EQ(getConstRValueRefResult.Value(), 123);
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (int32_t&&) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (int32_t&&) with passed type: const int32_t*");
}

TEST(Variant, Const_RValueRef) {
    const auto& variant = Variant::Create<const int&&>(123);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<detail::ConstRValueRefVariantWrapper<int>>(variant));

    const auto getValueResult = variant.Get<int>();
    const auto getConstValueResult = variant.Get<const int>();
    const auto getLValueRefResult = variant.Get<int&>();
    const auto getConstLValueRefResult = variant.Get<const int&>();
    const auto getRValueRefResult = variant.Get<int&&>();
    const auto getConstRValueRefResult = variant.Get<const int&&>();
    const auto getPointerResult = variant.Get<int*>();
    const auto getConstPointerResult = variant.Get<const int*>();

    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (const int32_t&&) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get from Variant (const int32_t&&) with passed type: const int32_t");
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get from Variant (const int32_t&&) with passed type: int32_t&");
    ASSERT_EQ(getConstLValueRefResult.Error().Message(), "cannot get from Variant (const int32_t&&) with passed type: const int32_t&");
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get from Variant (const int32_t&&) with passed type: int32_t&&");
    ASSERT_EQ(getConstRValueRefResult.Value(), 123);
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (const int32_t&&) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (const int32_t&&) with passed type: const int32_t*");
}

TEST(Variant, Pointer) {
    int value = 123;
    const auto& variant = Variant::Create<int*>(&value);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<detail::PointerVariantWrapper<int>>(variant));

    const auto getValueResult = variant.Get<int>();
    const auto getConstValueResult = variant.Get<const int>();
    const auto getLValueRefResult = variant.Get<int&>();
    const auto getConstLValueRefResult = variant.Get<const int&>();
    const auto getRValueRefResult = variant.Get<int&&>();
    const auto getConstRValueRefResult = variant.Get<const int&&>();
    const auto getPointerResult = variant.Get<int*>();
    const auto getConstPointerResult = variant.Get<const int*>();

    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (int32_t*) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get from Variant (int32_t*) with passed type: const int32_t");
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get from Variant (int32_t*) with passed type: int32_t&");
    ASSERT_EQ(getConstLValueRefResult.Error().Message(), "cannot get from Variant (int32_t*) with passed type: const int32_t&");
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get from Variant (int32_t*) with passed type: int32_t&&");
    ASSERT_EQ(getConstRValueRefResult.Error().Message(), "cannot get from Variant (int32_t*) with passed type: const int32_t&&");
    ASSERT_EQ(getPointerResult.Value(), &value);
    ASSERT_EQ(getConstPointerResult.Value(), &value);
}

TEST(Variant, Const_Pointer) {
    constexpr int value = 123;
    const auto& variant = Variant::Create<const int*>(&value);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<detail::ConstPointerVariantWrapper<int>>(variant));

    const auto getValueResult = variant.Get<int>();
    const auto getConstValueResult = variant.Get<const int>();
    const auto getLValueRefResult = variant.Get<int&>();
    const auto getConstLValueRefResult = variant.Get<const int&>();
    const auto getRValueRefResult = variant.Get<int&&>();
    const auto getConstRValueRefResult = variant.Get<const int&&>();
    const auto getPointerResult = variant.Get<int*>();
    const auto getConstPointerResult = variant.Get<const int*>();

    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (const int32_t*) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get from Variant (const int32_t*) with passed type: const int32_t");
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get from Variant (const int32_t*) with passed type: int32_t&");
    ASSERT_EQ(getConstLValueRefResult.Error().Message(), "cannot get from Variant (const int32_t*) with passed type: const int32_t&");
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get from Variant (const int32_t*) with passed type: int32_t&&");
    ASSERT_EQ(getConstRValueRefResult.Error().Message(), "cannot get from Variant (const int32_t*) with passed type: const int32_t&&");
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (const int32_t*) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Value(), &value);
}
}

struct TestStruct {
    int value = 42;
};

template <>
struct ReflCpp::ReflectData<TestStruct> {
    static Result<TypeData> Create() {
        return TypeData { .name = "TestStruct" };
    }
};

// Helper function to create variants for testing
template <typename T>
ReflCpp::Variant CreateVariant(T value) {
    return ReflCpp::Variant::Create<T>(std::forward<T>(value));
}

TEST(VariantTests, BasicValueGet) {
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
TEST(VariantTests, ConstValueGet) {
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
TEST(VariantTests, ReferenceGet) {
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
TEST(VariantTests, ConstReferenceGet) {
    int value = 42;
    const int& ref = value;
    auto variant = CreateVariant(ref);

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
TEST(VariantTests, PointerGet) {
    int value = 42;
    int* ptr = &value;
    auto variant = CreateVariant(ptr);

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
TEST(VariantTests, ConstPointerGet) {
    int value = 42;
    const int* ptr = &value;
    auto variant = CreateVariant(ptr);

    EXPECT_FALSE(variant.CanGet<int*>()); // Can't get non-const from const
    EXPECT_TRUE(variant.CanGet<const int*>());

    EXPECT_TRUE(variant.Get<int*>().HasError());
    EXPECT_EQ(*variant.Get<const int*>().Value(), 42);

    // Modifying the original should be visible through the variant
    value = 100;
    EXPECT_EQ(*variant.Get<const int*>().Value(), 100);
}

// Test rvalue handling
TEST(VariantTests, RValueGet) {
    auto variant = CreateVariant(42);

    EXPECT_TRUE(variant.CanGet<int>());
    EXPECT_TRUE(variant.CanGet<const int>());
    EXPECT_TRUE(variant.CanGet<int&>());
    EXPECT_TRUE(variant.CanGet<const int&>());

    EXPECT_EQ(variant.Get<int>().Value(), 42);
    EXPECT_EQ(variant.Get<const int>().Value(), 42);
    EXPECT_EQ(variant.Get<int&>().Value(), 42);
    EXPECT_EQ(variant.Get<const int&>().Value(), 42);
}

// Test with custom class
TEST(VariantTests, CustomClassGet) {
    TestStruct obj;
    obj.value = 42;
    auto variant = CreateVariant(obj);

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
TEST(VariantTests, VoidVariant) {
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
TEST(VariantTests, TypeMismatch) {
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
TEST(VariantTests, RValueReference) {
    int value = 42;
    const auto variant = CreateVariant(std::move(value));

    // Test CanGet for rvalue reference
    EXPECT_TRUE(variant.CanGet<int&&>());

    // Test actual Get for rvalue reference
    EXPECT_EQ(variant.Get<int&&>().Value(), 42);
}

// Test complex type conversions to ensure unified implementation works
TEST(VariantTests, ComplexTypeConversions) {
    TestStruct obj;
    obj.value = 42;

    // Create variants with different reference/pointer types
    const auto v1 = CreateVariant(obj); // value
    const auto v2 = CreateVariant(std::ref(obj)); // reference_wrapper
    const auto v3 = CreateVariant(&obj); // pointer
    const auto v4 = CreateVariant(std::move(obj)); // rvalue

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
    EXPECT_EQ(v3.Get<TestStruct*>().Value()->value, 100);
    EXPECT_EQ(v2.Get<TestStruct&>().Value().value, 100);
}
