#pragma once

#include "refl-cpp/method.hpp"

#include "refl-cpp/common/list_seperator.hpp"
#include "refl-cpp/reflect.hpp"

namespace ReflCpp {
inline void Method::Print(std::ostream& stream) const {
    Reflect(m_Func.GetReturnType()).Print(stream);
    stream << " " << m_Name << "(";

    ListSeperator sep{};
    const auto& arg_types = m_Func.GetArgumentsTypes();
    for (int i = 0; i < m_ArgumentNames.size(); ++i) {
        stream << sep;
        
        auto& arg_name = m_ArgumentNames[i];
        stream << arg_name << ": ";

        auto& arg_type = arg_types[i];
        stream << Reflect(arg_type);
    }

    stream << ")";
}

inline std::string Method::Dump() const {
    std::stringstream stream;
    Print(stream);
    return stream.str();
}

}
