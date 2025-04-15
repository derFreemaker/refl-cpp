#pragma once

#include "res-cpp/res-cpp.h"

namespace ReflCpp {
template <typename T>
using Result = ResCpp:esult<T, ResCpp::FormattedError>;
}
