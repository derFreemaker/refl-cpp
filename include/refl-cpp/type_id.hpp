#pragma once

#include <cstdint>

namespace ReflCpp {

struct TypeID {
    const uint32_t id;

    operator uint32_t() const {
        return id;
    }
    
    TypeID() = delete;
    
    constexpr TypeID(const uint32_t id)
        : id(id) {}

    bool operator==(const TypeID other) const {
        return id == other.id;
    }
};

}