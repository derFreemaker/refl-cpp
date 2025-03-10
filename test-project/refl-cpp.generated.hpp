#pragma once

#include "refl-cpp/refl-cpp.hpp"

namespace kpop {
struct Test;
}

struct Test2;

namespace ReflCpp {
template <>
struct ReflectData<kpop::Test> : ReflectTypeData {
    static ReflectTypeData Create() {
        return {
            .name = "Test",
            ._namespace = "kpop",
            .methods = {
                {
                    .ptr = static_cast<void()>(&kpop::Test::foo),
                    .name = "foo",
                    .return_type = Reflect<void>(),
                },
            },
        };
    }
};

template <>
struct ReflectData<Test2> : ReflectTypeData {
    static ReflectTypeData Create() {
        return {
            .name = "Test2",
            .bases = {
                Reflect<kpop::Test>()
            }
        };
    }
};
}
