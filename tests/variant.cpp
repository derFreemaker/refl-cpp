#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "refl-cpp/variant.hpp"
#include "refl-cpp/impl/variant.hpp"

#include "helper/result_helper.hpp"
#include "helper/variant_helper.hpp"

namespace ReflCpp {

//TODO: create custom exceptions or and result type for error handling

TEST(Variant, Void) {
    const auto& variant_void = Variant::Void();

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<VoidVariantWrapper>(variant_void));

    const auto getValueResult = variant_void.GetValue<int>();
    const auto getRefResult = variant_void.GetRef<int>();
    
    ASSERT_TRUE(ResultTestHelper::IsError(getValueResult, "cannot get reference or value from void variant"));
    ASSERT_TRUE(ResultTestHelper::IsError(getRefResult, "cannot get reference or value from void variant"));
}

TEST(Variant, Value) {
    const auto variant_constant = Variant(123);
    
    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ValueVariantWrapper<int>>(variant_constant));
    
    ASSERT_EQ(variant_constant.GetValue<const int>().Value(), 123);
    ASSERT_EQ(variant_constant.GetValue<int>().Value(), 123);
    
    ASSERT_EQ(variant_constant.GetRef<const int>().Value(), 123);

    const auto constantRefResult = variant_constant.GetRef<int>();
    ASSERT_TRUE(ResultTestHelper::IsError(constantRefResult, "cannot get modifiable reference to constant"));
}

TEST(Variant, Reference) {
    auto int_ref = std::make_unique<int>(123);
    const auto variant_ref = Variant(int_ref);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<RefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
    
    ASSERT_EQ(variant_ref.GetValue<std::unique_ptr<int>>().Value(), int_ref);
    ASSERT_EQ(variant_ref.GetRef<std::unique_ptr<int>>().Value(), int_ref);
}

TEST(Variant, ConstReference) {
    const auto int_ref = std::make_unique<int>(123);
    const auto variant_ref = Variant(int_ref);

    ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
    
    ASSERT_EQ(variant_ref.GetValue<const std::unique_ptr<int>>().Value(), int_ref);
    ASSERT_EQ(variant_ref.GetValue<std::unique_ptr<int>>().Value(), int_ref);
    
    ASSERT_EQ(variant_ref.GetRef<const std::unique_ptr<int>>().Value(), int_ref);

    const auto constantRefRefResult = variant_ref.GetRef<std::unique_ptr<int>>();
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

    const auto constantPointerRefResultWithConst = variant_ref.GetRef<const int*>();
    ASSERT_TRUE(ResultTestHelper::IsError(constantPointerRefResultWithConst, "cannot get modifiable reference to constant"));

    const auto constantPointerRefResult = variant_ref.GetRef<int*>();
    ASSERT_TRUE(ResultTestHelper::IsError(constantPointerRefResult, "cannot get modifiable reference to constant"));
}

}