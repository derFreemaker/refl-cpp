#include "refl-cpp/argument.hpp"

#include "refl-cpp/reflect.hpp"

namespace ReflCpp {
template <typename T>
Argument::Argument(T& data)
    : m_Data(&data),
      m_Type(Reflect<T>()) {}

const Type& Argument::GetType() const {
    return Reflect(m_Type);
}
}
