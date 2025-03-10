#include "refl-cpp/type.hpp"

#include "refl-cpp/reflect.hpp"

namespace ReflCpp {
const Type& Type::GetInner() const {
    return Reflect(m_Inner.value());
}
}
