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
