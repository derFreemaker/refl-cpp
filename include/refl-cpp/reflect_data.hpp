#pragma once

#include <vector>
#include <optional>

#include "refl-cpp/method_data.hpp"
#include "refl-cpp/type_id.hpp"

namespace ReflCpp {

struct ReflectTypeData {
    const char* name = "$NONE$";
    std::optional<const char*> _namespace = std::nullopt;

    std::vector<TypeID> bases;
    std::optional<TypeID> inner = std::nullopt;

    bool is_array = false;
    bool is_pointer = false;
    bool is_reference = false;
    bool is_const = false;
    bool is_volatile = false;
    
    std::vector<MethodData> methods;
};

template <typename T>
struct ReflectData;

}
