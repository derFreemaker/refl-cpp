#pragma once

#include <ostream>

#include "refl-cpp/type_data.hpp"
#include "refl-cpp/reflect_printer.hpp"

#define REFLECT_BUILTIN_TYPE_IMPL(TEMPLATE, TYPE, NAME, PRINT) \
    template <TEMPLATE> \
    struct ReflCpp::ReflectData<TYPE> { \
        static Result<TypeData> Create() { \
            return TypeData { .name = NAME }; \
        } \
    }; \
    template <TEMPLATE> \
    struct ReflCpp::ReflectPrinter<TYPE> { \
        static void Print(std::ostream& stream, const Type& type) { \
            stream << PRINT; \
        } \
    };

#define REFLECT_BUILTIN_TYPE(TYPE, NAME) \
    REFLECT_BUILTIN_TYPE_IMPL( ,TYPE, NAME, #TYPE)

REFLECT_BUILTIN_TYPE(void, "Void")

REFLECT_BUILTIN_TYPE(bool, "Boolean")

REFLECT_BUILTIN_TYPE(char, "Character")

REFLECT_BUILTIN_TYPE(float, "Float")

REFLECT_BUILTIN_TYPE(double, "Double")

REFLECT_BUILTIN_TYPE_IMPL(size_t Size_, char[Size_], "String", "char[" << Size_ << "]") // NOLINT(*-avoid-c-arrays)

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
