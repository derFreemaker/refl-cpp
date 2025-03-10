#include "refl-cpp/type.hpp"

#include "refl-cpp/refl-cpp.hpp"

namespace ReflCpp {
const Type& Type::GetInner() const {
    return Reflect(m_Inner.value());
}
}
