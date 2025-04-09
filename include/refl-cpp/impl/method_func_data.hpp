#pragma once

#include "refl-cpp/method_func_data.hpp"
#include "refl-cpp/method_func.hpp"

namespace ReflCpp {
template <typename T_>
MethodFuncData<T_>::operator std::unique_ptr<MethodFunc>() const {
    return std::unique_ptr(static_cast<MethodFunc*>(new MethodFuncWrapper<T_>(*this)));
}
}
