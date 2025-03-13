#include "test.generated.hpp"
#include "test.hpp"

namespace ReflCpp {
TypeData ReflectData<kpop::Test>::Create() {
    return TypeData {
        .name = "Test",
        ._namespace = "kpop",
        .fields = {
            FieldData {
                .ptr = &kpop::Test::test,
                .name = "test",
            }
        },
        .methods = {
            MethodData {
                .ptr = &kpop::Test::foo,
                .name = "foo",
                .arguments = {
                    "str",
                    "test",
                }
            },
        },
    };
}
}
