#include <catch2/catch_test_macros.hpp>

#include "helper/variant_helper.hpp"

namespace ReflCpp {
struct TestStruct {
    int value = 42;
};
}

REFLCPP_REFLECT_TEMPLATE()
REFLCPP_REFLECT_DATA(ReflCpp::TestStruct)
{ .name = "TestStruct" }
REFLCPP_REFLECT_DATA_END()

namespace ReflCpp::testing {

TEST_CASE("Variant Tests", "[variant]") {
    SECTION("Void") {
        const auto& variant = Variant::Void();

        CHECK_FALSE(variant.CanGet<int>());
        CHECK_FALSE(variant.CanGet<const int>());
        CHECK_FALSE(variant.CanGet<int&>());
        CHECK_FALSE(variant.CanGet<const int&>());

        CHECK(variant.Get<int>().has_error());
        CHECK(variant.Get<const int>().has_error());
        CHECK(variant.Get<int&>().has_error());
        CHECK(variant.Get<const int&>().has_error());

        // Check that it is actually a void variant
        CHECK(variant.GetType().Equals<void>());
    }

    SECTION("Value") {
        int value = 42;
        const auto variant = Variant::Create<int>(value).value();

        CHECK(variant.CanGet<int>());
        CHECK(variant.CanGet<const int>());
        CHECK(variant.CanGet<int&>());
        CHECK(variant.CanGet<const int&>());

        CHECK(variant.Get<int>().value() == 42);
        CHECK(variant.Get<const int>().value() == 42);
        CHECK(variant.Get<int&>().value() == 42);
        CHECK(variant.Get<const int&>().value() == 42);

        // Modifying through reference should modify the original
        variant.Get<int&>().value() = 100;
        CHECK(variant.Get<int>().value() == 100);
    }

    SECTION("ConstValue") {
        constexpr int value = 42;
        const auto variant = Variant::Create<const int>(value).value();

        CHECK_FALSE(variant.CanGet<int>()); // Can't get non-const from const
        CHECK(variant.CanGet<const int>());
        CHECK_FALSE(variant.CanGet<int&>()); // Can't get non-const from const
        CHECK(variant.CanGet<const int&>());

        CHECK(variant.Get<const int>().value() == 42);
        CHECK(variant.Get<const int&>().value() == 42);
    }

    SECTION("LValueReference") {
        int value = 42;
        const auto variant = Variant::Create<int&>(value).value();

        CHECK(variant.CanGet<int>());
        CHECK(variant.CanGet<const int>());
        CHECK(variant.CanGet<int&>());
        CHECK(variant.CanGet<const int&>());

        CHECK(variant.Get<int>().value() == 42);
        CHECK(variant.Get<const int>().value() == 42);
        CHECK(variant.Get<int&>().value() == 42);
        CHECK(variant.Get<const int&>().value() == 42);

        // Modifying through reference should modify the original
        variant.Get<int&>().value() = 100;
        CHECK(value == 100);
        CHECK(variant.Get<int>().value() == 100);
    }

    SECTION("ConstLValueReference") {
        int value = 42;
        const auto variant = Variant::Create<const int&>(value).value();

        CHECK_FALSE(variant.CanGet<int>()); // Can't get non-const from const
        CHECK(variant.CanGet<const int>());
        CHECK_FALSE(variant.CanGet<int&>()); // Can't get non-const from const
        CHECK(variant.CanGet<const int&>());

        CHECK(variant.Get<const int>().value() == 42);
        CHECK(variant.Get<const int&>().value() == 42);

        // Modifying the original should be visible through the variant
        value = 100;
        CHECK(variant.Get<const int>().value() == 100);
    }

    SECTION("RValueReference") {
        const auto v1 = Variant::Create<int&&>(42).value();

        CHECK(v1.CanGet<int&&>());
        CHECK(v1.CanGet<const int&&>());
        CHECK(v1.Get<int&&>().value() == 42);

        const auto v2 = Variant::Create<const int&&>(42).value();
        CHECK_FALSE(v2.CanGet<int&&>());
        CHECK(v2.CanGet<const int&&>());
        CHECK(v2.Get<const int&&>().value() == 42);
    }

    SECTION("ConstRValueReference") {
        int value = 42;
        const auto variant = Variant::Create<const int&&>(std::move(value)).value();

        // Test CanGet for rvalue reference
        CHECK(variant.CanGet<const int&&>());

        // Test actual Get for rvalue reference
        CHECK(variant.Get<const int&&>().value() == 42);
    }

    SECTION("Pointer") {
        int value = 42;
        const auto variant = Variant::Create<int*>(&value).value();

        CHECK(variant.CanGet<int*>());
        CHECK(variant.CanGet<const int*>());

        CHECK(variant.Get<int*>().value() == &value);
        CHECK(variant.Get<const int*>().value() == &value);

        // Modifying through pointer should modify the original
        *variant.Get<int*>().value() = 100;
        CHECK(value == 100);
        CHECK(variant.Get<int*>().value() == &value);
    }

    SECTION("ConstPointer") {
        int value = 42;
        const auto variant = Variant::Create<const int*>(&value).value();

        CHECK_FALSE(variant.CanGet<int*>()); // Can't get non-const from const
        CHECK(variant.CanGet<const int*>());

        CHECK(variant.Get<const int*>().value() == &value);

        // Modifying the original should be visible through the variant
        value = 100;
        CHECK(*variant.Get<const int*>().value() == 100);
    }

    SECTION("CustomClassGet") {
        TestStruct obj;
        obj.value = 42;
        const auto variant = Variant::Create<TestStruct>(obj).value();

        CHECK(variant.CanGet<TestStruct>());
        CHECK(variant.CanGet<const TestStruct>());
        CHECK(variant.CanGet<TestStruct&>());
        CHECK(variant.CanGet<const TestStruct&>());

        CHECK(variant.Get<TestStruct>().value().value == 42);
        CHECK(variant.Get<const TestStruct>().value().value == 42);
        CHECK(variant.Get<TestStruct&>().value().value == 42);
        CHECK(variant.Get<const TestStruct&>().value().value == 42);

        // Modifying through reference should modify the variant's copy
        variant.Get<TestStruct&>().value().value = 100;
        CHECK(variant.Get<TestStruct>().value().value == 100);
    }

    SECTION("TypeMismatch") {
        int value = 42;
        const auto variant = Variant::Create<int>(value).value();

        // Test CanGet with wrong types
        CHECK_FALSE(variant.CanGet<double>());
        CHECK_FALSE(variant.CanGet<std::string>());
        CHECK_FALSE(variant.CanGet<TestStruct>());

        // Test actual Get with wrong types
        CHECK(variant.Get<double>().has_error());
        CHECK(variant.Get<std::string>().has_error());
        CHECK(variant.Get<TestStruct>().has_error());
    }

    SECTION("ComplexTypeConversions") {
        TestStruct obj;
        obj.value = 42;

        // Create variants with different reference/pointer types
        const auto v1 = Variant::Create<TestStruct>(obj).value(); // value
        const auto v2 = Variant::Create<TestStruct&>(obj).value(); // reference
        const auto v3 = Variant::Create<TestStruct*>(&obj).value(); // pointer
        const auto v4 = Variant::Create<TestStruct&&>(static_cast<TestStruct&&>(obj)).value(); // rvalue

        // Check value copy
        CHECK(v1.CanGet<TestStruct>());
        CHECK(v1.Get<TestStruct>().value().value == 42);

        // Check reference wrapper
        CHECK(v2.CanGet<TestStruct&>());
        CHECK(v2.Get<TestStruct&>().value().value == 42);

        // Check pointer
        CHECK(v3.CanGet<TestStruct*>());
        CHECK(v3.Get<TestStruct*>().value()->value == 42);

        // Check rvalue
        CHECK(v4.CanGet<TestStruct&&>());
        CHECK(v4.Get<TestStruct&&>().value().value == 42);

        // Modify original and check pointer reflects change
        obj.value = 100;
        CHECK(v2.Get<TestStruct&>().value().value == 100);
        CHECK(v3.Get<TestStruct*>().value()->value == 100);
    }
}
} // namespace ReflCpp
