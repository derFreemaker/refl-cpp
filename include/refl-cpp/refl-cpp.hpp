#pragma once

#include "refl-cpp/reflect.hpp"

#ifdef REFLECT_PARSER
#define REFLECT() __attribute__((annotate("reflect")))
#else
#define REFLECT()
#endif

#include "impl/impl.hpp"
