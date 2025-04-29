#include <catch2/catch_test_macros.hpp>
#include <string>
#include <memory>

#include "refl-cpp/type.hpp"
#include "refl-cpp/field.hpp"
#include "refl-cpp/method.hpp"
#include "refl-cpp/variant.hpp"

// Test classes
namespace TestClasses {
class SimpleClass {
private:
    int privateValue = 10;

public:
    int publicValue = 20;
    static int staticValue;

    const int constValue = 30;
    std::string name = "Default";

    int getValue() const {
        return privateValue;
    }

    void setValue(int val) {
        privateValue = val;
    }

    static int getStaticValue() {
        return staticValue;
    }

    static void setStaticValue(int val) {
        staticValue = val;
    }

    int add(int a, int b) const {
        return a + b;
    }

    void setName(const std::string& newName) {
        name = newName;
    }

    std::string getName() const {
        return name;
    }
};

int SimpleClass::staticValue = 50;

// Nested class for testing inner type reflection
class ContainerClass {
public:
    class InnerClass {
    public:
        int value = 100;
    };

    InnerClass inner;
};
} // namespace TestClasses

template <>
struct ReflCpp::ReflectData<TestClasses::SimpleClass> {
    static TypeData Create() {
        return TypeData{
            .name = "SimpleClass",
            ._namespace = "TestClasses",
            .fields = {
                FieldData{
                    .ptr = &TestClasses::SimpleClass::publicValue,
                    .name = "publicValue",
                },
                FieldData{
                    .ptr = &TestClasses::SimpleClass::constValue,
                    .name = "constValue",
                },
                FieldData{
                    .ptr = &TestClasses::SimpleClass::name,
                    .name = "name",
                },
                FieldData{
                    .ptr = &TestClasses::SimpleClass::staticValue,
                    .name = "staticValue",
                }
            },
            .methods = {
                MethodData{
                    .name = "getValue",
                    .funcs = {
                        MethodFuncData{
                            .ptr = &TestClasses::SimpleClass::getValue,
                        },
                    },
                },
                MethodData{
                    .name = "setValue",
                    .funcs = {
                        MethodFuncData{
                            .ptr = &TestClasses::SimpleClass::setValue,
                            .args = { "val" },
                        },
                    },
                },
            },
        };
    }
};

// Reflection declarations
// .method("getValue", &TestClasses::SimpleClass::getValue)
// .method("setValue", &TestClasses::SimpleClass::setValue)
// .method("getStaticValue", &TestClasses::SimpleClass::getStaticValue)
// .method("setStaticValue", &TestClasses::SimpleClass::setStaticValue)
// .method("add", &TestClasses::SimpleClass::add)
// .method("getName", &TestClasses::SimpleClass::getName)
// .method("setName", &TestClasses::SimpleClass::setName);

// REFLECT_TYPE(TestClasses::ContainerClass);
// REFLECT_TYPE(TestClasses::ContainerClass::InnerClass)
//     .field("value", &TestClasses::ContainerClass::InnerClass::value);

TEST_CASE("Field reflection tests", "[field]") {
    TestClasses::SimpleClass testInstance;
    const ReflCpp::Type& testType = ReflCpp::Reflect<TestClasses::SimpleClass>();
    ReflCpp::Variant instanceVariant = ReflCpp::Variant::Create<TestClasses::SimpleClass&>(testInstance);

    SECTION("GetFields count") {
        // Verify that we get the correct number of fields
        REQUIRE(testType.GetFields().size() == 5);
    }

    SECTION("GetField by name") {
        // Test getting field by name
        auto publicValueField = testType.GetField("publicValue");
        REQUIRE(publicValueField.has_value());

        auto nonExistentField = testType.GetField("nonExistent");
        REQUIRE_FALSE(nonExistentField.has_value());
    }

    SECTION("GetValue for instance field") {
        auto publicValueField = testType.GetField("publicValue");
        REQUIRE(publicValueField.has_value());

        // Get value through reflection
        auto value = publicValueField->get().GetValue(instanceVariant);
        REQUIRE(value.Get<int>() == 20);
    }

    SECTION("GetValue for static field") {
        auto staticValueField = testType.GetField("staticValue");
        REQUIRE(staticValueField.has_value());

        // Get static value without instance
        auto value = staticValueField->get().GetValue();
        REQUIRE(value.Get<int>() == 50);
    }

    SECTION("SetValue for instance field") {
        auto publicValueField = testType.GetField("publicValue");
        REQUIRE(publicValueField.has_value());

        // Create a new value
        ReflCpp::Variant newValue = ReflCpp::Variant::Create<int>(42);

        // Set the value through reflection
        publicValueField->get().SetValue(newValue, instanceVariant);

        // Verify the value was changed
        REQUIRE(testInstance.publicValue == 42);

        // Also verify through GetValue
        auto retrievedValue = publicValueField->get().GetValue(instanceVariant);
        REQUIRE(retrievedValue.Get<int>() == 42);
    }

    SECTION("SetValue for static field") {
        auto staticValueField = testType.GetField("staticValue");
        REQUIRE(staticValueField.has_value());

        // Save original value to restore later
        int originalValue = TestClasses::SimpleClass::staticValue;

        // Create a new value
        ReflCpp::Variant newValue = ReflCpp::Variant::Create<int>(123);

        // Set the static value through reflection
        staticValueField->get().SetValue(newValue);

        // Verify the value was changed
        REQUIRE(TestClasses::SimpleClass::staticValue == 123);

        // Also verify through GetValue
        auto retrievedValue = staticValueField->get().GetValue();
        REQUIRE(retrievedValue.Get<int>() == 123);

        // Restore original value
        TestClasses::SimpleClass::staticValue = originalValue;
    }

    SECTION("GetType for field") {
        auto publicValueField = testType.GetField("publicValue");
        REQUIRE(publicValueField.has_value());

        // Get the type of the field
        auto typeId = publicValueField->get().GetType();

        // Verify it's correct
        REQUIRE(typeId == ReflCpp::ReflectID<int>());
    }

    SECTION("GetRef for field") {
        auto nameField = testType.GetField("name");
        REQUIRE(nameField.has_value());

        // Get reference to field
        auto& nameRef = nameField->get().GetRef<std::string>(instanceVariant);

        // Modify through reference
        nameRef = "Modified";

        // Verify that original was modified
        REQUIRE(testInstance.getName() == "Modified");
    }

    SECTION("const field access") {
        auto constValueField = testType.GetField("constValue");
        REQUIRE(constValueField.has_value());

        // Can read const value
        auto value = constValueField->get().GetValue(instanceVariant);
        REQUIRE(value.Get<int>() == 30);

        // Cannot set const value (this should throw)
        ReflCpp::Variant newValue = ReflCpp::Variant::Create<int>(999);
        REQUIRE_THROWS_AS(constValueField->get().SetValue(newValue, instanceVariant), std::logic_error);
    }
}

TEST_CASE("Method reflection tests", "[method]") {
    TestClasses::SimpleClass testInstance;
    const ReflCpp::Type& testType = ReflCpp::Reflect<TestClasses::SimpleClass>();
    ReflCpp::Variant instanceVariant = ReflCpp::Variant::Create<TestClasses::SimpleClass&>(testInstance);

    SECTION("GetMethods count") {
        // Verify that we get the correct number of methods
        REQUIRE(testType.GetMethods().size() == 7);
    }

    SECTION("GetMethod by name") {
        // Test getting method by name
        auto getValue = testType.GetMethod("getValue");
        REQUIRE(getValue.has_value());

        auto nonExistentMethod = testType.GetMethod("nonExistent");
        REQUIRE_FALSE(nonExistentMethod.has_value());
    }

    SECTION("Invoke instance method with no parameters") {
        auto getValue = testType.GetMethod("getValue");
        REQUIRE(getValue.has_value());

        // Set a specific value first
        testInstance.setValue(42);

        // Invoke the method
        ReflCpp::ArgumentList noArgs;
        auto result = getValue->get().Invoke(instanceVariant, noArgs);

        // Check result
        REQUIRE(result.has_value());
        REQUIRE(result.value().Get<int>() == 42);
    }

    SECTION("Invoke static method with no parameters") {
        auto getStaticValue = testType.GetMethod("getStaticValue");
        REQUIRE(getStaticValue.has_value());

        // Set a specific value first
        TestClasses::SimpleClass::setStaticValue(123);

        // Invoke the static method
        ReflCpp::ArgumentList noArgs;
        auto result = getStaticValue->get().Invoke(Variant::Void(), noArgs);

        // Check result
        REQUIRE(result.has_value());
        REQUIRE(result.value().Get<int>() == 123);
    }

    SECTION("Invoke method with parameters") {
        auto add = testType.GetMethod("add");
        REQUIRE(add.has_value());

        // Create arguments list
        ReflCpp::ArgumentList args;
        args.push_back(ReflCpp::Variant::Create<int>(10));
        args.push_back(ReflCpp::Variant::Create<int>(32));

        // Invoke the method
        auto result = add->get().Invoke(instanceVariant, args);

        // Check result
        REQUIRE(result.has_value());
        REQUIRE(result.value().Get<int>() == 42);
    }

    SECTION("Invoke void method with parameters") {
        auto setValue = testType.GetMethod("setValue");
        REQUIRE(setValue.has_value());

        // Create arguments list
        ReflCpp::ArgumentList args;
        args.push_back(ReflCpp::Variant::Create<int>(75));

        // Invoke the method
        auto result = setValue->get().Invoke(instanceVariant, args);

        // Check that the method executed correctly
        REQUIRE(testInstance.getValue() == 75);
    }

    SECTION("Invoke static void method with parameters") {
        auto setStaticValue = testType.GetMethod("setStaticValue");
        REQUIRE(setStaticValue.has_value());

        // Save original value to restore later
        int originalValue = TestClasses::SimpleClass::staticValue;

        // Create arguments list
        ReflCpp::ArgumentList args;
        args.push_back(ReflCpp::Variant::Create<int>(321));

        // Invoke the static method
        auto result = setStaticValue->get().Invoke(Variant::Void(), args);

        // Check that the method executed correctly
        REQUIRE(TestClasses::SimpleClass::staticValue == 321);

        // Restore original value
        TestClasses::SimpleClass::staticValue = originalValue;
    }

    SECTION("Invoke method with string parameter") {
        auto setName = testType.GetMethod("setName");
        REQUIRE(setName.has_value());

        // Create arguments list
        ReflCpp::ArgumentList args;
        args.push_back(ReflCpp::Variant::Create<std::string>("TestName"));

        // Invoke the method
        auto result = setName->get().Invoke(instanceVariant, args);

        // Check that the method executed correctly
        REQUIRE(testInstance.getName() == "TestName");
    }
}

TEST_CASE("Type inner class testing", "[type]") {
    const ReflCpp::Type& containerType = ReflCpp::Reflect<TestClasses::ContainerClass>();

    SECTION("Inner type access") {
        // Verify the container has inner types
        REQUIRE(containerType.HasInners());

        // Verify we can find the inner type
        bool foundInner = false;
        for (const auto& innerId : containerType.GetInners()) {
            auto innerTypeResult = innerId.GetType();
            if (innerTypeResult.is_ok()) {
                const auto& innerType = innerTypeResult.value();
                if (std::string(innerType.GetName()) == "InnerClass") {
                    foundInner = true;
                    break;
                }
            }
        }

        REQUIRE(foundInner);
    }

    SECTION("Inner type reflection") {
        const ReflCpp::Type& innerType = ReflCpp::Reflect<TestClasses::ContainerClass::InnerClass>();

        // Verify the inner type has the expected field
        auto valueField = innerType.GetField("value");
        REQUIRE(valueField.has_value());

        // Create an instance to test with
        TestClasses::ContainerClass::InnerClass innerInstance;
        innerInstance.value = 42;
        ReflCpp::Variant innerVariant = ReflCpp::Variant::Create<TestClasses::ContainerClass::InnerClass&>(innerInstance);

        // Verify field access works
        auto value = valueField->get().GetValue(innerVariant);
        REQUIRE(value.Get<int>() == 42);
    }
}
