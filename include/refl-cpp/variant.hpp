#pragma once

#include <stdexcept>

#include "refl-cpp/type_id.hpp"

namespace ReflCpp {
struct Variant {
private:
    void* m_Data;
    const TypeID m_Type;

public:
    Variant() = delete;

    template <typename T>
    Variant(T& data);

    Variant(void* data, const TypeID type_id)
        : m_Data(data), m_Type(type_id) {}

    [[nodiscard]]
    void* GetValue(const TypeID type) const {
        if (m_Type != type) {
            throw std::invalid_argument("Variant::GetData(TypeID) called with not stored type id");
        }
        return m_Data;
    }
    
    template <typename T>
    [[nodiscard]]
    T* GetValue() const;
};
}
