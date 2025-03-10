#include "refl-cpp/method.hpp"

#include "refl-cpp/reflect.hpp"

namespace ReflCpp {
const Type& Method::GetReturnType() const {
    if (m_ReturnType.IsValid()) {
        return Reflect(m_ReturnType);
    }

    return Reflect<void>();
}
}
