#pragma once

#include <cstdint>

namespace ReflCpp {
struct TypeID {
    static const TypeID Invalid;

    const uint32_t id = Invalid;

    operator uint32_t() const {
        return id;
    }

    constexpr TypeID(const uint32_t id)
        : id(id) {}

    bool operator==(const TypeID other) const {
        return id == other.id;
    }

    [[nodiscard]]
    bool IsValid() const {
        return *this != Invalid;
    }

    [[nodiscard]]
    bool IsInvalid() const {
        return *this == Invalid;
    }
};
}
