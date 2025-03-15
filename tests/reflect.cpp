#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "refl-cpp/refl-cpp.hpp"

using namespace ReflCpp;

TEST(ReflectionDatabase, RegisterType) {
    const TypeID typeID = TypeInstance<void>::ID();
    const TypeID reflectedTypeID = ReflectID<void>();
    const Type& reflectedType = Reflect<void>();

    EXPECT_EQ(typeID, reflectedTypeID);
    EXPECT_EQ(typeID, reflectedType.GetID());
}

//TODO: add more tests
