#pragma once

#include "refl-cpp/method_func_data.hpp"
#include "refl-cpp/method_func.hpp"

namespace ReflCpp {
template <typename T>
MethodFuncData<T>::operator std::shared_ptr<MethodFunc>() const {
    return std::make_shared<MethodFuncWrapper<T>>(*this);
}
}
