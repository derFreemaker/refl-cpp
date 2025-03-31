#pragma once

#include <cstdint>

#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct TypeID {
private:
    static constexpr auto m_Invalid = 0;

    uint32_t m_ID = m_Invalid;

public:
    static constexpr auto Invalid() {
        return TypeID(m_Invalid);
    }

    operator uint32_t() const {
        return m_ID;
    }
    
    constexpr TypeID(const uint32_t id)
        : m_ID(id) {}

    bool operator==(const TypeID other) const {
        return m_ID == other.m_ID;
    }

    [[nodiscard]]
    bool IsValid() const {
        return this->m_ID != m_Invalid;
    }

    [[nodiscard]]
    bool IsInvalid() const {
        return this->m_ID == m_Invalid;
    }

    [[nodiscard]]
    Result<const Type&> GetType() const {
        if (IsInvalid()) {
            return { RESULT_ERROR(), "invalid type id" };
        }

        return detail::Reflect(m_ID);
    }
    
    operator Result<const Type&>() const {
        return GetType();
    }

    template <typename T_>
    [[nodiscard]]
    bool Equals() const {
        const auto result = ReflectID<T_>();
        return !result.HasError() && *this == result.Value();
    }
};
}
