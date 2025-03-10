#pragma once

#include "refl-cpp/reflect.hpp"

#include "refl-cpp/meta/builtin.hpp"
#include "refl-cpp/meta/common.hpp"
#include "refl-cpp/meta/std.hpp"

#define REFLECT() __attribute__((annotate("reflect")))
