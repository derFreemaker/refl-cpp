#include "test.generated.hpp"
#include "../../test.hpp"

namespace ReflCpp {
ReflectTypeData ReflectData<kpop::Test>::Create() {
    return {
        .name = "Test",
        ._namespace = "kpop",
        .methods = {
            MethodData {
                .name = "foo",
                .func = &kpop::Test::foo,
                .arguments = {
                    "str",
                    "test",
                }
            },
        },
    };
}
}
