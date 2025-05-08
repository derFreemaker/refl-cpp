#pragma once

#include "refl-cpp/type_data.hpp"
#include "refl-cpp/reflect_printer.hpp"

#define REFLECT_BUILTIN_TYPE_IMPL(TYPE, NAME, PRINT) \
    REFLCPP_REFLECT_TEMPLATE() \
    REFLCPP_REFLECT_DATA_DECL(TYPE) \
    REFLCPP_REFLECT_DATA_DEF(TYPE) \
        { .name = NAME } \
    REFLCPP_REFLECT_DATA_DEF_END(); \
    REFLCPP_REFLECT_TEMPLATE() \
    REFLCPP_REFLECT_PRINTER(TYPE) \
            out << PRINT; \
    REFLCPP_REFLECT_PRINTER_END()

#define REFLECT_BUILTIN_TYPE(TYPE, NAME) \
    REFLECT_BUILTIN_TYPE_IMPL(TYPE, NAME, #TYPE)

REFLECT_BUILTIN_TYPE(bool, "Boolean")

REFLECT_BUILTIN_TYPE(char, "Character")

REFLECT_BUILTIN_TYPE(float, "Float")

REFLECT_BUILTIN_TYPE(double, "Double")

REFLCPP_REFLECT_TEMPLATE(size_t Size_)
REFLCPP_REFLECT_DATA_DECL(char[Size_])

REFLCPP_REFLECT_TEMPLATE(size_t Size_)
REFLCPP_REFLECT_DATA_DEF(char[Size_]) // NOLINT(*-avoid-c-arrays)
    {
        .name = "Char Array"
    }
REFLCPP_REFLECT_DATA_DEF_END()

REFLCPP_REFLECT_TEMPLATE(size_t Size_)
REFLCPP_REFLECT_PRINTER(char[Size_]) // NOLINT(*-avoid-c-arrays)
    out << "char[" << Size_ << "]";
REFLCPP_REFLECT_PRINTER_END()

REFLECT_BUILTIN_TYPE(uint8_t, "Unsigned 8 Bit Integer")

REFLECT_BUILTIN_TYPE(uint16_t, "Unsigned 16 Bit Integer")

REFLECT_BUILTIN_TYPE(uint32_t, "Unsigned 32 Bit Integer")

REFLECT_BUILTIN_TYPE(uint64_t, "Unsigned 64 Bit Integer")

REFLECT_BUILTIN_TYPE(int8_t, "Unsigned 8 Bit Integer")

REFLECT_BUILTIN_TYPE(int16_t, "Unsigned 16 Bit Integer")

REFLECT_BUILTIN_TYPE(int32_t, "Unsigned 32 Bit Integer")

REFLECT_BUILTIN_TYPE(int64_t, "Unsigned 64 Bit Integer")

#undef REFLECT_BUILTIN_TYPE
#undef REFLECT_BUILTIN_TYPE_IMPL
