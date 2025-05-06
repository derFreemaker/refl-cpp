#pragma once

#include "refl-cpp/declare_reflect.hpp"

// std::string

REFLCPP_REFLECT_TEMPLATE()
REFLCPP_REFLECT_DATA(std::string){
    .name = "string",
    ._namespace = "std"
}
REFLCPP_REFLECT_DATA_END()
