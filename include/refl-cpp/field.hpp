#pragma once

#include "refl-cpp/field_wrapper.hpp"
#include "refl-cpp/field_data.hpp"
#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct Field {
private:
    FieldBase* const m_Base;

    const char* m_Name;

public:
    template <typename T_>
    Field(const FieldData<T_>& data)
        : m_Base(new FieldWrapper<T_>(data.ptr)),
          m_Name(data.name) {}

    [[nodiscard]]
    const char* GetName() const {
        return m_Name;
    }

    [[nodiscard]]
    TypeID GetType() const {
        return m_Base->GetType();
    }

    [[nodiscard]]
    Variant GetValueStatic() const {
        return m_Base->GetValueStatic();
    }

    void SetValueStatic(Variant& value) const {
        m_Base->SetValueStatic(value);
    }

    [[nodiscard]]
    Variant GetValue(const Variant& instance) const {
        return m_Base->GetValue(instance);
    }

    void SetValue(const Variant& instance, const Variant& value) const {
        return m_Base->SetValue(instance, value);
    }
};
}
