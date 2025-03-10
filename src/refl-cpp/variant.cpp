#include "refl-cpp/variant.hpp"

#include "refl-cpp/reflect.hpp"

namespace ReflCpp {

template <typename T>
Variant::Variant(T& data)
    : m_Data(&data),
      m_Type(Reflect<T>()) {}

template <typename T>
T* Variant::GetValue() const {
    if (m_Type != Reflect<T>()) {
        throw std::invalid_argument("Variant::GetData() with not stored type");
    }
    return m_Data;
}
}
