#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "refl-cpp/common/result.hpp"

#include "../helper/result_helper.hpp"

namespace ReflCpp {
TEST(Result, Error) {
    auto testError = []() -> Result<void> {
        return { Error, "some error: {0}", 2345 };
    };
    
    const auto resultTestError = testError();

    ASSERT_TRUE(resultTestError.IsError());
    ASSERT_FALSE(resultTestError.IsSuccess());
    ASSERT_EQ(resultTestError.Error().Str(), "some error: 2345");
}

TEST(Result, ErrorPropagation) {
    auto testError = []() -> Result<void> {
        return { Error, "some error: {0}", 2345 };
    };

    auto testErrorPropagation = [&testError]() -> Result<void> {
        TRY(testError());
        return { Error, "different error" };
    };

    const auto resultTestErrorPropagation = testErrorPropagation();

    ASSERT_TRUE(ResultTestHelper::IsError(resultTestErrorPropagation, "some error: 2345"));
}

TEST(Result, Void) {
    auto testSuccess = []() -> Result<void> {
        return { Ok };
    };

    const auto resultTestSuccess = testSuccess();

    ASSERT_TRUE(resultTestSuccess.IsSuccess());
    ASSERT_FALSE(resultTestSuccess.IsError());
}

TEST(Result, Value) {
    auto testValue = []() -> Result<int> {
        // also testing convertion
        // from type T2_ (aka 'float') to type T_ (aka 'int') 
        return { Ok, 123.5f };
    };

    const auto resultValue = testValue();

    ASSERT_EQ(resultValue.Value(), 123);
}

TEST(Result, Reference) {
    int testInt = 3456;
    auto testReference = [&testInt]() -> Result<int&> {
        return { Ok, testInt };
    };

    const auto resultReference = testReference();

    ASSERT_EQ(resultReference.Value(), testInt);
}

TEST(Result, ConstReference) {
    constexpr int testInt = 786345;
    auto testReference = [&testInt]() -> Result<const int&> {
        return { Ok, testInt };
    };

    const auto resultReference = testReference();

    ASSERT_EQ(resultReference.Value(), testInt);
}

TEST(Result, Pointer) {
    int testInt = 87345;

    auto testReference = [&testInt]() -> Result<int*> {
        return { Ok, &testInt };
    };

    const auto resultReference = testReference();

    ASSERT_EQ(*resultReference.Value(), testInt);
}

TEST(Result, ConstPointer) {
    constexpr int testInt = 12893;

    auto testReference = [&testInt]() -> Result<const int*> {
        return { Ok, &testInt };
    };

    const auto resultReference = testReference();

    ASSERT_EQ(*resultReference.Value(), testInt);
}

}
