#include <gtest/gtest.h>

#include "refl-cpp/common/result.hpp"
#include "helper/variant_helper.hpp"

namespace ReflCpp {
//TODO: check tests through maybe not exactly right

#define CREATE_VARIANT_TEST(NAME, WRAPPER, TYPE, ...) \
    TEST(Variant, NAME) { \
        __VA_ARGS__; \
        ASSERT_TRUE(VariantTestHelper::UsesWrapper<WRAPPER>(variant)); \
        const auto getValueResult = variant.Get<TYPE>(); \
        const auto getConstValueResult = variant.Get<const TYPE>(); \
        const auto getLValueRefResult = variant.Get<TYPE&>(); \
        const auto getConstLValueRefResult = variant.Get<const TYPE&>(); \
        const auto getRValueRefResult = variant.Get<TYPE&&>(); \
        const auto getConstRValueRefResult = variant.Get<const TYPE&&>(); \
        const auto getPointerResult = variant.Get<TYPE*>(); \
        const auto getConstPointerResult = variant.Get<const TYPE*>();

CREATE_VARIANT_TEST(Void, detail::VoidVariantWrapper, int,
                    const auto& variant = Variant::Void()
)
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getValueResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getConstLValueRefResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getConstRValueRefResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get reference or value from void variant");
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get reference or value from void variant");
}

CREATE_VARIANT_TEST(Value, detail::ValueVariantWrapper<int>, int,
                    const auto& variant = Variant::Create<int>(123)
)
    ASSERT_EQ(getValueResult.Value(), 123);
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get from Variant (int32_t) with passed type: const int32_t");
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get from Variant (int32_t) with passed type: int32_t&");
    ASSERT_EQ(getConstLValueRefResult.Error().Message(), "cannot get from Variant (int32_t) with passed type: const int32_t&");
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get from Variant (int32_t) with passed type: int32_t&&");
    ASSERT_EQ(getConstRValueRefResult.Error().Message(), "cannot get from Variant (int32_t) with passed type: const int32_t&&");
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (int32_t) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (int32_t) with passed type: const int32_t*");
}

CREATE_VARIANT_TEST(Const_Value, detail::ConstValueVariantWrapper<int>, int,
                    const auto& variant = Variant::Create<const int>(123)
)
    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Value(), 123);
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: int32_t&");
    ASSERT_EQ(getConstLValueRefResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: const int32_t&");
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: int32_t&&");
    ASSERT_EQ(getConstRValueRefResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: const int32_t&&");
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (const int32_t) with passed type: const int32_t*");
}

CREATE_VARIANT_TEST(LValueRef, detail::LValueRefVariantWrapper<int>, int,
                    int value = 123;
                    const auto& variant = Variant::Create<int&>(value)
)
    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: const int32_t");
    ASSERT_EQ(getLValueRefResult.Value(), 123);
    ASSERT_EQ(getConstLValueRefResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: const int32_t&");
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: int32_t&&");
    ASSERT_EQ(getConstRValueRefResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: const int32_t&&");
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (int32_t&) with passed type: const int32_t*");
}

CREATE_VARIANT_TEST(Const_LValueRef, detail::ConstLValueRefVariantWrapper<int>, int,
                    const auto& variant = Variant::Create<const int&>(123)
)
    ASSERT_EQ(getValueResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: int32_t");
    ASSERT_EQ(getConstValueResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: const int32_t");
    ASSERT_EQ(getLValueRefResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: int32_t&");
    ASSERT_EQ(getConstLValueRefResult.Value(), 123);
    ASSERT_EQ(getRValueRefResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: int32_t&&");
    ASSERT_EQ(getConstRValueRefResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: const int32_t&&");
    ASSERT_EQ(getPointerResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: int32_t*");
    ASSERT_EQ(getConstPointerResult.Error().Message(), "cannot get from Variant (const int32_t&) with passed type: const int32_t*");
}

// TEST(Variant, Value) {
//     const auto variant_constant = Variant::Create<int>(123);
//
//     ASSERT_TRUE(VariantTestHelper::UsesWrapper<ValueVariantWrapper<int>>(variant_constant));
//
//     ASSERT_EQ(variant_constant.Get<const int>().Value(), 123);
//     ASSERT_EQ(variant_constant.Get<int>().Value(), 123);
//
//     ASSERT_EQ(variant_constant.Get<const int&>().Value(), 123);
//
//     const auto constantRefResult = variant_constant.Get<int&>();
//     ASSERT_EQ(constantRefResult.Error().Message(), "cannot get modifiable reference to constant");
// }
//
// TEST(Variant, LValueReference) {
//     auto int_ref = std::make_unique<int>(123);
//     const auto variant_ref = Variant::Create(int_ref);
//
//     ASSERT_TRUE(VariantTestHelper::UsesWrapper<LValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
//
//     ASSERT_EQ(variant_ref.Get<std::unique_ptr<int>>().Value(), int_ref);
//     ASSERT_EQ(variant_ref.Get<std::unique_ptr<int>&>().Value(), int_ref);
// }
//
// TEST(Variant, ConstLValueReference) {
//     const auto int_ref = std::make_unique<int>(123);
//     const auto variant_ref = Variant::Create(int_ref);
//
//     ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstLValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
//
//     ASSERT_EQ(variant_ref.Get<const std::unique_ptr<int>>().Value(), int_ref);
//
//     const auto constantRefValueResult = variant_ref.Get<std::unique_ptr<int>>();
//     ASSERT_EQ(constantRefValueResult.Error().Message(), "passed type 'std::unique_ptr<int32_t>' is not the same as the stored type 'const std::unique_ptr<int32_t>&'");
//
//     ASSERT_EQ(variant_ref.Get<const std::unique_ptr<int>&>().Value(), int_ref);
//
//     const auto constantRefRefResult = variant_ref.Get<std::unique_ptr<int>&>();
//     ASSERT_EQ(constantRefRefResult.Error().Message(), "cannot get modifiable reference to constant");
// }
//
// TEST(Variant, RValueReference) {
//     constexpr auto int_value = 2134234;
//     auto int_ref = std::make_unique<int>(int_value);
//     const auto variant_ref = Variant::Create(std::move(int_ref));
//
//     ASSERT_TRUE(VariantTestHelper::UsesWrapper<RValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
//
//     ASSERT_EQ(*variant_ref.Get<std::unique_ptr<int>&&>().Value(), int_value);
//     ASSERT_EQ(*variant_ref.GetRef<std::unique_ptr<int>&&>().Value(), int_value);
// }
//
// TEST(Variant, ConstRValueReference) {
//     constexpr auto int_value = 2134234;
//     const auto int_ref = std::make_unique<int>(int_value);
//     auto create_test_value = [&int_ref]() -> const std::unique_ptr<int>&& {
//         return std::move(int_ref);
//     };
//     const auto variant_ref = Variant::Create(std::move(create_test_value()));
//
//     ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstRValueRefVariantWrapper<std::unique_ptr<int>>>(variant_ref));
//
//     ASSERT_EQ(*variant_ref.GetValue<const std::unique_ptr<int>&&>().Value(), int_value);
//
//     const auto constantRefValueResult = variant_ref.GetValue<std::unique_ptr<int>&&>();
//     ASSERT_EQ(constantRefValueResult.Error().Message(), "passed type 'std::unique_ptr<int32_t>&&' is not the same as the stored type 'const std::unique_ptr<int32_t>&&'");
//
//     ASSERT_EQ(*variant_ref.GetRef<const std::unique_ptr<int>&&>().Value(), int_value);
//
//     const auto constantRefRefResult = variant_ref.GetRef<std::unique_ptr<int>&&>();
//     ASSERT_EQ(constantRefRefResult.Error().Message(), "cannot get modifiable reference to constant");
// }
//
// TEST(Variant, Pointer) {
//     int int_field = 123;
//     const auto variant_ref = Variant::Create(&int_field);
//
//     ASSERT_TRUE(VariantTestHelper::UsesWrapper<PointerVariantWrapper<int>>(variant_ref));
//
//     ASSERT_EQ(*variant_ref.GetValue<int*>().Value(), int_field);
//     ASSERT_EQ(*variant_ref.GetRef<int*>().Value(), int_field);
// }
//
// TEST(Variant, ConstPointer) {
//     constexpr int int_field = 123;
//     const auto variant_ref = Variant::Create(&int_field);
//
//     ASSERT_TRUE(VariantTestHelper::UsesWrapper<ConstPointerVariantWrapper<int>>(variant_ref));
//
//     ASSERT_EQ(variant_ref.GetValue<const int*>().Value(), &int_field);
//     ASSERT_EQ(variant_ref.GetValue<int*>().Value(), &int_field);
//
//     ASSERT_EQ(variant_ref.GetRef<const int*>().Value(), &int_field);
//
//     const auto constantPointerRefResult = variant_ref.GetRef<int*>();
//     ASSERT_EQ(constantPointerRefResult.Error().Message(), "cannot get reference from Variant (const int32_t*) with passed type: int32_t*");
// }
}
