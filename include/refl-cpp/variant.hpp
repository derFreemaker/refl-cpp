#pragma once

#include <stdexcept>
#include <any>

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct Variant {
private:
    std::any m_Data;
    const TypeID m_Type;

public:
    Variant() = delete;

    template <typename T>
        requires (!std::is_same_v<T, Variant>)
    Variant(T& data)
        : m_Data(data),
          m_Type(ReflectID<T>()) {}

    Variant(void* data, const TypeID type_id)
        : m_Data(data), m_Type(type_id) {}

    template <typename T>
    [[nodiscard]]
    T GetValue() const {
        return std::any_cast<T>(m_Data);
    }

    [[nodiscard]]
    const Type& GetType() const {
        return Reflect(m_Type);
    }
};
}
