#include "test.generated.hpp"
#include "test.hpp"

ReflCpp::Result<ReflCpp::TypeData> ReflCpp::ReflectData<kpop::Test>::Create() {
    return TypeData{
        .name = "Test",
        ._namespace = "kpop",
        .fields = {
            TRY(Field::Create(FieldData{
                .ptr = &kpop::Test::test,
                .name = "test",
            })),
        },
        .methods = {
            MethodData{
                .name = "foo",
                .funcs = {
                    MethodFuncData<void(kpop::Test::*)(const char*)>{
                        .ptr = &kpop::Test::foo,
                        .args = { "_foo" },
                    },
                    MethodFuncData<void(kpop::Test::*)(int)>{
                        .ptr = &kpop::Test::foo,
                        .args = { "_foo" },
                    },
                },
            },
            //     MethodData{
            //         .ptr = &kpop::Test::foo,
            //         .name = "foo",
            //         .arguments = {
            //             "_foo",
            //         }
            //     },
        },
    };
}
