#pragma once

#include <vector>

namespace ReflCpp {
template <typename Func_>
struct MethodFuncData {
    Func_ ptr;
    std::vector<const char*> args{};
};

template <typename... Funcs_>
struct MethodData {
    const char* name = "$NONE$";
    std::tuple<MethodFuncData<Funcs_>...> funcs;

    MethodData(const char* name, MethodFuncData<Funcs_>... fs)
        : name(name), funcs(fs...) {}
};
}
