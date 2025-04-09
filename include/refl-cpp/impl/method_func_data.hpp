#pragma once

#include "refl-cpp/method_func_data.hpp"
#include "refl-cpp/method_func.hpp"

namespace ReflCpp {
template <typename T_>
MethodFuncData<T_>::operator std::shared_ptr<MethodFunc>() const noexcept {
    return std::make_shared<MethodFuncWrapper<T_>>(*this);
}
}
