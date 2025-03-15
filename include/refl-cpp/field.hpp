#pragma once

#include <memory>

#include "refl-cpp/field_wrapper.hpp"
#include "refl-cpp/field_data.hpp"

namespace ReflCpp {
struct Field {
private:
    std::shared_ptr<FieldBase> m_Base;

    const char* m_Name;

public:
    template <typename T_>
    Field(const FieldData<T_>& data)
        : m_Base(std::make_shared<FieldWrapper<T_>>(data.ptr)),
          m_Name(data.name) {}

    [[nodiscard]]
    const char* GetName() const {
        return m_Name;
    }

    [[nodiscard]]
    TypeID GetType() const {
        return m_Base->GetType();
    }

    void SetValue(const Variant& value, const Variant& instance = Variant::Void()) const {
        return m_Base->SetValue(value, instance);
    }

    [[nodiscard]]
    Variant GetValue(const Variant& instance = Variant::Void()) const {
        return m_Base->GetValue(instance);
    }

    template <typename T_>
    [[nodiscard]]
    T_ GetValue(const Variant& instance = Variant::Void()) const {
        return m_Base->GetValue(instance).GetValue<T_>();
    }
    
    [[nodiscard]]
    Variant GetRef(const Variant& instance = Variant::Void()) const {
        return m_Base->GetRef(instance);
    }
    
    template <typename T_>
    [[nodiscard]]
    T_& GetRef(const Variant& instance = Variant::Void()) const {
        return m_Base->GetRef(instance).GetRef<T_>();
    }
};
}
