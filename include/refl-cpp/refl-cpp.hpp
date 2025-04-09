#pragma once

#include "refl-cpp/reflect.hpp"

#ifdef REFLECT_PARSER
#define REFLECT() [[reflect]]
#else
#define REFLECT()
#endif

#include "impl/impl.hpp"
