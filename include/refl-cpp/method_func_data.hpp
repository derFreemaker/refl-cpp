#pragma once

#include <vector>

namespace ReflCpp {

struct MethodFunc;

template <typename T_>
struct MethodFuncData {
    T_ ptr;
    std::vector<const char*> args{};

    operator std::shared_ptr<MethodFunc>() const;
};
}
