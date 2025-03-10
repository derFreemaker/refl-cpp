#pragma once

#include <ostream>

#include "refl-cpp/reflect_data.hpp"
#include "refl-cpp/reflect_printer.hpp"

#define REFLECT_BUILTIN_TYPE(TYPE, NAME) \
    template <> \
    struct ReflCpp::ReflectData<TYPE> { \
        static ReflectTypeData Create() { \
            return { .name = NAME }; \
        } \
    }; \
    template <> \
    struct ReflCpp::ReflectPrinter<TYPE> { \
        static void Print(std::ostream& stream, const Type& type) { \
            stream << #TYPE; \
        } \
    };

REFLECT_BUILTIN_TYPE(void, "Void")
REFLECT_BUILTIN_TYPE(bool, "Boolean")
REFLECT_BUILTIN_TYPE(char, "Character")

REFLECT_BUILTIN_TYPE(uint8_t, "Unsigned 8 Bit Integer")
REFLECT_BUILTIN_TYPE(uint16_t, "Unsigned 16 Bit Integer")
REFLECT_BUILTIN_TYPE(uint32_t, "Unsigned 32 Bit Integer")
REFLECT_BUILTIN_TYPE(uint64_t, "Unsigned 64 Bit Integer")

REFLECT_BUILTIN_TYPE(int8_t, "Unsigned 8 Bit Integer")
REFLECT_BUILTIN_TYPE(int16_t, "Unsigned 16 Bit Integer")
REFLECT_BUILTIN_TYPE(int32_t, "Unsigned 32 Bit Integer")
REFLECT_BUILTIN_TYPE(int64_t, "Unsigned 64 Bit Integer")

#undef REFLECT_BUILTIN_TYPE