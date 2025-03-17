#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "refl-cpp/refl-cpp.hpp"

namespace ReflCpp {

struct TestStruct {
    int normal;
    const int constant;
    static int static_normal;
    static int static_constant;
};

int TestStruct::static_normal = -567;
int TestStruct::static_constant = 278;

template <>
struct ReflectData<TestStruct> {
    static TypeData Create() {
        return {
            .name = "TestStruct",
            .fields = {
                FieldData {
                    .ptr = &TestStruct::normal,
                    .name = "normal"
                },
                FieldData {
                    .ptr = &TestStruct::constant,
                    .name = "constant"
                },
                FieldData {
                    .ptr = &TestStruct::static_normal,
                    .name = "static_normal"
                },
                FieldData {
                    .ptr = &TestStruct::static_constant,
                    .name = "static_constant"
                },
            },
        };
    }
};

TEST(Fields, GetValue) {
    TestStruct test{
        .normal = 123,
        .constant = 42,
    };

    const auto& type = Reflect<TestStruct>();
    
    const auto& field_normal = type.GetField("normal")->get();
    const auto& field_constant = type.GetField("constant")->get();
    const auto& field_static_normal = type.GetField("static_normal")->get();
    const auto& field_static_constant = type.GetField("static_constant")->get();
    
    ASSERT_EQ(field_normal.GetValue<int>(test), 123);
    ASSERT_EQ(field_normal.GetRef<int>(test), 123);
    
    ASSERT_EQ(field_constant.GetValue<const int>(test), 42);
    ASSERT_THROW((void)field_constant.GetRef<const int>(test), std::logic_error);

    ASSERT_EQ(field_static_normal.GetValue<int>(test), -567);
    ASSERT_EQ(field_static_normal.GetRef<int>(test), -567);
    
    ASSERT_EQ(field_static_constant.GetValue<const int>(test), 278);
    ASSERT_THROW((void)field_constant.GetRef<const int>(test), std::logic_error);
}

}
