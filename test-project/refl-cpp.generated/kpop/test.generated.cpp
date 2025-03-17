#include "test.generated.hpp"
#include "test.hpp"

ReflCpp::TypeData ReflCpp::ReflectData<kpop::Test>::Create() {
    return {
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
                    "_foo",
                }
            },
        },
    };
}
