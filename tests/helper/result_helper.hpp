#pragma once

#include <string>

#include <gtest/gtest.h>

#include "refl-cpp/common/result.hpp"

namespace ReflCpp {
struct ResultTestHelper {
    template <typename T_>
    static bool IsError(const Result<T_>& result, std::string errMsg) {
        if (!result.IsError() || result.IsSuccess()) {
            return false;
        }
        
        if (result.Error().Message() != errMsg) {
            return false;
        }

        return true;
    }
};
}
