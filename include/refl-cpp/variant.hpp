#pragma once

#include <stdexcept>

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct Variant {
private:
    void* m_Data;
    const TypeID m_Type;

public:
    Variant() = delete;

    template <typename T>
        requires (!std::is_same_v<T, Variant>)
    Variant(T& data)
        : m_Data((void*)&data),
          m_Type(ReflectID<T>()) {}

    Variant(void* data, const TypeID type_id)
        : m_Data(data), m_Type(type_id) {}

    [[nodiscard]]
    void* GetValue(const TypeID type) const {
        if (m_Type != type) {
            throw std::invalid_argument("Variant::GetValue(TypeID) called with not stored type id");
        }
        return m_Data;
    }

    template <typename T>
    [[nodiscard]]
    T* GetValue() const {
        if (m_Type != ReflectID<T>()) {
            throw std::invalid_argument("Variant::GetValue() with not stored type");
        }

        return static_cast<T*>(m_Data);
    }

    [[nodiscard]]
    void* GetDataRaw() const {
        return m_Data;
    }
    
};
}
