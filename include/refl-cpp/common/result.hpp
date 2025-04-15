#pragma once

#include "res-cpp/res-cpp.h"

namespace ReflCpp {
template <typename T>
using Result = ResCpp::Result<T, ResCpp::FormattedError>;
}
