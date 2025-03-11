#pragma once

#include <cstdint>

namespace ReflCpp {
struct TypeID {
private:
    static constexpr auto m_Invalid = 0;
    
    uint32_t id = m_Invalid;

public:
    static constexpr auto Invalid() {
        return TypeID(m_Invalid);
    }
    
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
        return this->id != m_Invalid;
    }

    [[nodiscard]]
    bool IsInvalid() const {
        return this->id == m_Invalid;
    }
};
}
