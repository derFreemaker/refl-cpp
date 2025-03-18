#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "refl-cpp/refl-cpp.hpp"

namespace ReflCpp {

struct VariantTestHelper {
    template <typename Wrapper_>
    static Wrapper_* GetWrapper(const Variant& variant) {
        return dynamic_cast<Wrapper_*>(variant.m_Base.get());
    }

    template <typename Wrapper_>
    static bool IsWrapper(const Variant& variant) {
        return GetWrapper<Wrapper_>(variant) != nullptr;
    }
};

//TODO: create custom exceptions or and result type for error handling

TEST(Variant, Void) {
    const auto variant_void = Variant::Void();

    ASSERT_TRUE(VariantTestHelper::IsWrapper<VoidVariantWrapper>(variant_void));

    // It doesn't matter what type we pass since we should check for void before doing anything.
    ASSERT_THROW((void)variant_void.GetValue<int>(), std::logic_error);
    ASSERT_THROW((void)variant_void.GetRef<const char*>(), std::logic_error);
}

TEST(Variant, Value) {
    const auto variant_constant = Variant(123);
    
    ASSERT_TRUE(VariantTestHelper::IsWrapper<ValueVariantWrapper<int>>(variant_constant));
    
    ASSERT_EQ(variant_constant.GetValue<const int>(), 123);
    ASSERT_EQ(variant_constant.GetValue<int>(), 123);
    
    ASSERT_EQ(variant_constant.GetRef<const int>(), 123);
    ASSERT_THROW((void)variant_constant.GetRef<int>(), std::logic_error);
}

TEST(Variant, Reference) {
    auto int_ref = std::make_unique<int>(123);
    const auto variant_ref = Variant(int_ref);

    ASSERT_TRUE(VariantTestHelper::IsWrapper<RefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
    
    ASSERT_EQ(variant_ref.GetValue<std::unique_ptr<int>>(), int_ref);
    ASSERT_EQ(variant_ref.GetRef<std::unique_ptr<int>>(), int_ref);
}

TEST(Variant, ConstReference) {
    const auto int_ref = std::make_unique<int>(123);
    const auto variant_ref = Variant(int_ref);

    ASSERT_TRUE(VariantTestHelper::IsWrapper<ConstRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
    
    ASSERT_EQ(variant_ref.GetValue<const std::unique_ptr<int>>(), int_ref);
    ASSERT_EQ(variant_ref.GetValue<std::unique_ptr<int>>(), int_ref);
    
    ASSERT_EQ(variant_ref.GetRef<const std::unique_ptr<int>>(), int_ref);
    ASSERT_THROW((void)variant_ref.GetRef<std::unique_ptr<int>>(), std::logic_error);
}

TEST(Variant, Pointer) {
    int int_field = 123;
    const auto variant_ref = Variant(&int_field);

    ASSERT_TRUE(VariantTestHelper::IsWrapper<PointerVariantWrapper<int>>(variant_ref));

    ASSERT_EQ(*variant_ref.GetValue<int*>(), int_field);
    ASSERT_EQ(*variant_ref.GetRef<int*>(), int_field);
}

TEST(Variant, ConstPointer) {
    constexpr int int_field = 123;
    const auto variant_ref = Variant(&int_field);

    ASSERT_TRUE(VariantTestHelper::IsWrapper<ConstPointerVariantWrapper<int>>(variant_ref));

    ASSERT_EQ(variant_ref.GetValue<const int*>(), &int_field);
    ASSERT_EQ(variant_ref.GetValue<int*>(), &int_field);

    ASSERT_EQ(variant_ref.GetRef<const int*>(), &int_field);
    ASSERT_THROW((void)variant_ref.GetRef<int*>(), std::logic_error);
}

}