#include "refl-cpp/argument.hpp"

#include "refl-cpp/refl-cpp.hpp"
#include "refl-cpp/database.hpp"

namespace ReflCpp {
template <typename T>
Argument::Argument(const char* name)
    : m_Name(name), m_TypeID(Reflect<T>()) {}

const char* Argument::GetName() const {
    return m_Name;
}

const Type& Argument::GetType() const {
    return ReflectionDatabase::Instance().GetType(m_TypeID);
}
}
