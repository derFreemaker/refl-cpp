#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "refl-cpp/refl-cpp.hpp"

using namespace ReflCpp;

TEST(ReflectionDatabase, RegisterType) {
    const Type& reflectedType = Reflect<void>();
    const TypeID reflectedTypeID = ReflectID<void>();

    EXPECT_EQ(reflectedTypeID, reflectedType.GetID());
}
