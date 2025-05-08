#pragma once

#include <vector>

namespace ReflCpp {

struct MethodFunc;

template <typename T>
struct MethodFuncData {
    T ptr;
    std::vector<const char*> args{};

    operator std::shared_ptr<MethodFunc>() const;
};
}
