#include <gtest/gtest.h>

#include "helper/result_helper.hpp"
#include "helper/variant_helper.hpp"

namespace ReflCpp {

//TODO: check tests through maybe not exactly right

TEST(Variant, Void) {
    auto foo = Variant(Variant(123));
    
    const auto& variant_void = Variant::Void();

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<VoidVariantWrapper>(variant_void));

    const auto getValueResult = variant_void.GetValue<int>();
    const auto getRefResult = variant_void.GetRef<int>();
    
    ASSERT_TRUE(ResultTestHelper::IsError(getValueResult, "cannot get reference or value from void variant"));
    ASSERT_TRUE(ResultTestHelper::IsError(getRefResult, "cannot get reference or value from void variant"));
}

TEST(Variant, Value) {
    constexpr int int_value = 123;
    const auto variant_constant = Variant(int_value);
    
    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ValueVariantWrapper<int>>(variant_constant));
    
    ASSERT_EQ(variant_constant.GetValue<const int>().Value(), int_value);
    ASSERT_EQ(variant_constant.GetValue<int>().Value(), int_value);
    
    ASSERT_EQ(variant_constant.GetConstRef<const int>().Value(), int_value);

    const auto constantRefResult = variant_constant.GetRef<int>();
    ASSERT_TRUE(ResultTestHelper::IsError(constantRefResult, "cannot get modifiable reference to constant"));
}

TEST(Variant, LValueReference) {
    auto int_ref = std::make_unique<int>(123);
    const auto variant_ref = Variant(int_ref);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<LValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
    
    ASSERT_EQ(variant_ref.GetValue<std::unique_ptr<int>&>().Value(), int_ref);
    ASSERT_EQ(variant_ref.GetRef<std::unique_ptr<int>&>().Value(), int_ref);
}

TEST(Variant, ConstLValueReference) {
    const auto int_ref = std::make_unique<int>(123);
    const auto variant_ref = Variant(int_ref);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstLValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
    
    ASSERT_EQ(variant_ref.GetValue<const std::unique_ptr<int>&>().Value(), int_ref);

    const auto constantRefValueResult = variant_ref.GetValue<std::unique_ptr<int>&>();
    ASSERT_TRUE(ResultTestHelper::IsError(constantRefValueResult, "passed type 'std::unique_ptr<int32_t>&' is not the same as the stored type 'const std::unique_ptr<int32_t>&'"));
    
    ASSERT_EQ(variant_ref.GetRef<const std::unique_ptr<int>&>().Value(), int_ref);

    const auto constantRefRefResult = variant_ref.GetRef<std::unique_ptr<int>&>();
    ASSERT_TRUE(ResultTestHelper::IsError(constantRefRefResult, "cannot get modifiable reference to constant"));
}

TEST(Variant, RValueReference) {
    constexpr auto int_value = 2134234;
    auto int_ref = std::make_unique<int>(int_value);
    const auto variant_ref = Variant(std::move(int_ref));

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<RValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
    
    ASSERT_EQ(*variant_ref.GetValue<std::unique_ptr<int>&&>().Value(), int_value);
    ASSERT_EQ(*variant_ref.GetRef<std::unique_ptr<int>&&>().Value(), int_value);
}

TEST(Variant, ConstRValueReference) {
    constexpr auto int_value = 2134234;
    const auto int_ref = std::make_unique<int>(int_value);
    auto create_test_value = [&int_ref]() -> const std::unique_ptr<int>&& {
        return std::move(int_ref);
    };
    const auto variant_ref = Variant(create_test_value());

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstRValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
    
    ASSERT_EQ(*variant_ref.GetValue<const std::unique_ptr<int>&&>().Value(), int_value);

    const auto constantRefValueResult = variant_ref.GetValue<std::unique_ptr<int>&&>();
    ASSERT_TRUE(ResultTestHelper::IsError(constantRefValueResult, "passed type 'std::unique_ptr<int32_t>&&' is not the same as the stored type 'const std::unique_ptr<int32_t>&&'"));
    
    ASSERT_EQ(*variant_ref.GetRef<const std::unique_ptr<int>&&>().Value(), int_value);

    const auto constantRefRefResult = variant_ref.GetRef<std::unique_ptr<int>&&>();
    ASSERT_TRUE(ResultTestHelper::IsError(constantRefRefResult, "cannot get modifiable reference to constant"));
}

TEST(Variant, Pointer) {
    int int_field = 123;
    const auto variant_ref = Variant(&int_field);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<PointerVariantWrapper<int>>(variant_ref));

    ASSERT_EQ(*variant_ref.GetValue<int*>().Value(), int_field);
    ASSERT_EQ(*variant_ref.GetRef<int*>().Value(), int_field);
}

TEST(Variant, ConstPointer) {
    constexpr int int_field = 123;
    const auto variant_ref = Variant(&int_field);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstPointerVariantWrapper<int>>(variant_ref));

    ASSERT_EQ(variant_ref.GetValue<const int*>().Value(), &int_field);
    ASSERT_EQ(variant_ref.GetValue<int*>().Value(), &int_field);

    ASSERT_EQ(variant_ref.GetRef<const int*>().Value(), &int_field);

    const auto constantPointerRefResult = variant_ref.GetRef<int*>();
    ASSERT_TRUE(ResultTestHelper::IsError(constantPointerRefResult, "cannot get modifiable reference to constant"));
}
}