#include <gtest/gtest.h>

#include "refl-cpp/common/result.hpp"
#include "helper/variant_helper.hpp"

namespace ReflCpp {
//TODO: check tests through maybe not exactly right

TEST(Variant, Void) {
    const auto& variant_void = Variant::Void();

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<VoidVariantWrapper>(variant_void));

    const auto getValueResult = variant_void.GetValue<int>();
    ASSERT_EQ(getValueResult.Error().Message(), "cannot get reference or value from void variant");


    const auto getRefResult = variant_void.GetRef<int>();
    ASSERT_EQ(getRefResult.Error().Message(), "cannot get reference or value from void variant");
}

TEST(Variant, Value) {
    const auto variant_constant = Variant::Create(123);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ValueVariantWrapper<int>>(variant_constant));

    ASSERT_EQ(variant_constant.GetValue<const int>().Value(), 123);
    ASSERT_EQ(variant_constant.GetValue<int>().Value(), 123);

    ASSERT_EQ(variant_constant.GetConstRef<const int>().Value(), 123);

    const auto constantRefResult = variant_constant.GetRef<int>();
    ASSERT_EQ(constantRefResult.Error().Message(), "cannot get modifiable reference to constant");
}

TEST(Variant, LValueReference) {
    auto int_ref = std::make_unique<int>(123);
    const auto variant_ref = Variant::Create(int_ref);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<LValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));

    ASSERT_EQ(variant_ref.GetValue<std::unique_ptr<int>&>().Value(), int_ref);
    ASSERT_EQ(variant_ref.GetRef<std::unique_ptr<int>&>().Value(), int_ref);
}

TEST(Variant, ConstLValueReference) {
    const auto int_ref = std::make_unique<int>(123);
    const auto variant_ref = Variant::Create(int_ref);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstLValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));

    ASSERT_EQ(variant_ref.GetValue<const std::unique_ptr<int>&>().Value(), int_ref);

    const auto constantRefValueResult = variant_ref.GetValue<std::unique_ptr<int>&>();
    ASSERT_EQ(constantRefValueResult.Error().Message(), "passed type 'std::unique_ptr<int32_t>&' is not the same as the stored type 'const std::unique_ptr<int32_t>&'");

    ASSERT_EQ(variant_ref.GetRef<const std::unique_ptr<int>&>().Value(), int_ref);

    const auto constantRefRefResult = variant_ref.GetRef<std::unique_ptr<int>&>();
    ASSERT_EQ(constantRefRefResult.Error().Message(), "cannot get modifiable reference to constant");
}

TEST(Variant, RValueReference) {
    constexpr auto int_value = 2134234;
    auto int_ref = std::make_unique<int>(int_value);
    const auto variant_ref = Variant::Create(std::move(int_ref));

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
    const auto variant_ref = Variant::Create(std::move(create_test_value()));

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstRValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));

    ASSERT_EQ(*variant_ref.GetValue<const std::unique_ptr<int>&&>().Value(), int_value);

    const auto constantRefValueResult = variant_ref.GetValue<std::unique_ptr<int>&&>();
    ASSERT_EQ(constantRefValueResult.Error().Message(), "passed type 'std::unique_ptr<int32_t>&&' is not the same as the stored type 'const std::unique_ptr<int32_t>&&'");

    ASSERT_EQ(*variant_ref.GetRef<const std::unique_ptr<int>&&>().Value(), int_value);

    const auto constantRefRefResult = variant_ref.GetRef<std::unique_ptr<int>&&>();
    ASSERT_EQ(constantRefRefResult.Error().Message(), "cannot get modifiable reference to constant");
}

TEST(Variant, Pointer) {
    int int_field = 123;
    const auto variant_ref = Variant::Create(&int_field);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<PointerVariantWrapper<int>>(variant_ref));

    ASSERT_EQ(*variant_ref.GetValue<int*>().Value(), int_field);
    ASSERT_EQ(*variant_ref.GetRef<int*>().Value(), int_field);
}

TEST(Variant, ConstPointer) {
    constexpr int int_field = 123;
    const auto variant_ref = Variant::Create(&int_field);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstPointerVariantWrapper<int>>(variant_ref));

    ASSERT_EQ(variant_ref.GetValue<const int*>().Value(), &int_field);
    ASSERT_EQ(variant_ref.GetValue<int*>().Value(), &int_field);

    ASSERT_EQ(variant_ref.GetRef<const int*>().Value(), &int_field);

    const auto constantPointerRefResult = variant_ref.GetRef<int*>();
    ASSERT_EQ(constantPointerRefResult.Error().Message(), "cannot get reference from Variant (const int32_t*) with passed type: int32_t*");
}
}
