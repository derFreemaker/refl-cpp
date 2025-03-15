#pragma once

#include "refl-cpp/variant.hpp"
#include "refl-cpp/declare_reflect.hpp"
#include "refl-cpp/field_traits.hpp"

namespace ReflCpp {
struct FieldBase {
    virtual ~FieldBase() = default;

    [[nodiscard]]
    virtual bool IsStatic() const = 0;

    [[nodiscard]]
    virtual bool IsConst() const = 0;

    [[nodiscard]]
    virtual TypeID GetType() const = 0;

    [[nodiscard]]
    virtual Variant GetValue(const Variant& instance = Variant::Void()) const = 0;

    virtual void SetValue(const Variant& value, const Variant& instance = Variant::Void()) const = 0;

    [[nodiscard]]
    virtual Variant GetRef(const Variant& instance = Variant::Void()) const = 0;
};

template <typename Field_>
struct FieldWrapper final : public FieldBase {
private:
    Field_ m_Field;

public:
    using Traits = FieldTraits<Field_>;

    FieldWrapper(Field_ field)
        : m_Field(field) {}

    [[nodiscard]]
    constexpr bool IsStatic() const override {
        return Traits::IsStatic;
    }

    [[nodiscard]]
    constexpr bool IsConst() const override {
        return Traits::IsConst;
    }

    [[nodiscard]]
    TypeID GetType() const override {
        return ReflectID<typename Traits::Type>();
    }
    
    [[nodiscard]]
    Variant GetValue(const Variant& instance = Variant::Void()) const override {
        if constexpr (Traits::IsStatic) {
            return static_cast<typename Traits::Type>(*m_Field);
        }
        else {
            if (instance.IsVoid()) {
                throw std::invalid_argument("instance is needed for non-static member fields");
            }
            
            auto& obj = instance.GetRef<typename Traits::ClassType>();
            return static_cast<typename Traits::Type>(obj.*m_Field);
        }
    }

    void SetValue(const Variant& value, const Variant& instance = Variant::Void()) const override {
        if constexpr (Traits::IsConst) {
            throw std::logic_error("cannot set value on a const type.");
        }
        else if constexpr (Traits::IsStatic) {
            *m_Field = value.GetValue<typename Traits::Type>();
        }
        else {
            if (instance.IsVoid()) {
                throw std::invalid_argument("instance is needed for non-static member fields");
            }
            
            auto& obj = instance.GetRef<typename Traits::ClassType>();
            obj.*m_Field = value.GetValue<typename Traits::Type>();
        }
    }

    [[nodiscard]]
    Variant GetRef(const Variant& instance = Variant::Void()) const override {
        if constexpr (Traits::IsConst) {
            throw std::logic_error("cannot get reference value on a const type. 'GetValue()' will use a const reference if the field is const.");
        }
        else if constexpr (Traits::IsStatic) {
            return static_cast<typename Traits::Type&>(*m_Field);
        }
        else {
            if (instance.IsVoid()) {
                throw std::invalid_argument("instance is needed for non-static member fields");
            }
            
            auto& obj = instance.GetRef<typename Traits::ClassType>();
            return static_cast<typename Traits::Type&>(obj.*m_Field);
        }
    }
};
}
