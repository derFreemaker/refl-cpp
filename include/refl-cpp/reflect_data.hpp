#pragma once

#include <vector>
#include <optional>
#include <functional>

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
    
    //TODO: add some kind of print method
};

template <typename T>
struct ReflectData;

}
