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
    static Variant Void() {
        return Variant(nullptr, ReflectID<void>());
    }

    Variant() = delete;

    template <typename T>
        requires (!std::is_same_v<T, Variant>
            && sizeof(T) < 8)
    Variant(T data)
        : m_Data(data), m_Type(ReflectID<T>()) {}

    template <typename T>
        requires (!std::is_same_v<T, Variant>
        && sizeof(T) > 8)
    Variant(T& data)
        : m_Data(data),
          m_Type(ReflectID<T>()) {}

    Variant(void* data, const TypeID type_id)
        : m_Data(data), m_Type(type_id) {}

    template <typename T>
    [[nodiscard]]
    T GetValue() const {
        if (this->m_Type == Void().m_Type) {
            throw std::runtime_error("cannot get value from an void type.");
        }

        return std::any_cast<T>(m_Data);
    }

    [[nodiscard]]
    const Type& GetType() const {
        return Reflect(m_Type);
    }
};
}
