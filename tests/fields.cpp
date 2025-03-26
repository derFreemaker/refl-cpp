// #include <gtest/gtest.h>
//
// #include "helper/variant_helper.hpp"
// #include "refl-cpp/field_wrapper.hpp"
//
// namespace ReflCpp {
// struct NoCopyOrMoveStruct {
//     int foo = 893745;
//
//     NoCopyOrMoveStruct() = default;
//     NoCopyOrMoveStruct(NoCopyOrMoveStruct&) = delete;
//     NoCopyOrMoveStruct(const NoCopyOrMoveStruct&) = delete;
//     NoCopyOrMoveStruct(NoCopyOrMoveStruct&&) = delete;
//     NoCopyOrMoveStruct(const NoCopyOrMoveStruct&&) = delete;
// };
//
// template <>
// struct ReflectData<NoCopyOrMoveStruct> {
//     static Result<TypeData> Create() {
//         return TypeData { .name = "NoCopyOrMoveStruct" };
//     }
// };
//
// #define STRINGIFY(x) #x
//
// #define CREATE_FIELD(BEFORE_TYPE, TYPE, AFTER_TYPE, NAME, INIT) \
//     BEFORE_TYPE TYPE AFTER_TYPE NAME INIT;
//
// #define CREATE_FIELDS(AFTER_TYPE, NAME, INIT) \
//     CREATE_FIELD(, NoCopyOrMoveStruct, AFTER_TYPE, NAME, INIT) \
//     CREATE_FIELD(static, NoCopyOrMoveStruct, AFTER_TYPE, Static_##NAME, ) \
//     CREATE_FIELD(inline static, NoCopyOrMoveStruct, AFTER_TYPE, InlineStatic_##NAME, INIT) \
//     CREATE_FIELD(const, NoCopyOrMoveStruct, AFTER_TYPE, Const_##NAME, INIT) \
//     CREATE_FIELD(static constexpr, NoCopyOrMoveStruct, AFTER_TYPE, StaticConstExpr_##NAME, INIT)
// #define CREATE_FIELD_STRUCT_NAME(NAME) NAME##_Struct
//
// #define CREATE_FIELD_STRUCT(AFTER_TYPE, NAME, INIT) \
//     struct CREATE_FIELD_STRUCT_NAME(NAME) { \
//         CREATE_FIELDS(AFTER_TYPE, NAME, INIT) \
//     }; \
//     CREATE_FIELD(, NoCopyOrMoveStruct, AFTER_TYPE, CREATE_FIELD_STRUCT_NAME(NAME)::Static_##NAME, INIT) \
//     template <> \
//     struct ReflectData<NAME##_Struct> { \
//         static Result<TypeData> Create() { \
//             return TypeData { .name = STRINGIFY(NAME##_Struct) }; \
//         } \
//     };
//
// CREATE_FIELD_STRUCT(, Normal, {})
//
// NoCopyOrMoveStruct ReferenceStruct{};
// CREATE_FIELD_STRUCT(&, Reference, = ReferenceStruct)
//
// #define CREATE_FIELD_TEST(STRUCT_NAME, NAME) \
//     TEST(FieldWrapper, NAME) { \
//         const FieldWrapper fieldWrapper(&STRUCT_NAME::NAME); \
//     }
//
// TEST(FieldWrapper, Normal) {
//     const FieldWrapper fieldWrapper(&Normal_Struct::Normal);
//
//     Normal_Struct _struct;
//     ASSERT_EQ(fieldWrapper.GetValue(_struct).Value().GetValue<NoCopyOrMoveStruct>().Value().foo, 893745);
// }
// }
