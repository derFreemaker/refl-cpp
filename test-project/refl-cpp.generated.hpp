#pragma once

#include "refl-cpp/refl-cpp.hpp"

namespace kpop {
class Test;
}

struct Test2;

namespace ReflCpp {
template <>
struct ReflectData<kpop::Test> : ReflectTypeData {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Test",
            ._namespace = "kpop"
        };
    }
};

template <>
struct ReflectData<Test2> : ReflectTypeData {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Test2",
            .bases = {
                Reflect<kpop::Test>()
            }
        };
    }
};
}
