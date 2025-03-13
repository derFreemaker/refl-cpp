#pragma once

#include "refl-cpp/field_data.hpp"
#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct Field {
private:
    const char* m_Name;
    const TypeID m_Type;

public:
    template <typename T_>
    Field(const FieldData<T_>& data)
        : m_Name(data.name), m_Type(ReflectID<T_>()) {}
    
    [[nodiscard]]
    const char* GetName() const {
        return m_Name;
    }

    //TODO: add get_value and set_value

    [[nodiscard]]
    TypeID GetTypeID() const {
        return m_Type;
    }

    [[nodiscard]]
    const Type& GetType() const {
        return Reflect(m_Type);
    }
};
}
