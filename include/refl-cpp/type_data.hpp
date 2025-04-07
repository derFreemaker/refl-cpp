#pragma once

#include <vector>
#include <optional>

#include "refl-cpp/reflect_printer.hpp"
#include "refl-cpp/field.hpp"
#include "refl-cpp/method.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/type_flags.hpp"

namespace ReflCpp {

struct TypeData {
    const char* name = "$NONE$";
    std::optional<const char*> _namespace = std::nullopt;

    std::vector<TypeID> bases;
    std::vector<TypeID> inners;

    TypeFlags flags;
    
    std::vector<Field> fields;
    // std::vector<Method> methods;
};

struct TypeOptions {
    ReflectPrintFunc print_func = nullptr;
};

template <typename T>
struct ReflectData;
}
