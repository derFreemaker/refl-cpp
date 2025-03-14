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
    virtual Variant GetValueStatic() const = 0;

    virtual void SetValueStatic(Variant& value) const = 0;

    [[nodiscard]]
    virtual Variant GetValue(const Variant& instance) const = 0;

    virtual void SetValue(const Variant& instance, const Variant& value) const = 0;
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
    Variant GetValueStatic() const override {
        if constexpr (!Traits::IsStatic) {
            throw std::logic_error("cannot get static value on a non-static type.");
        }
        else {
            return *m_Field;
        }
    }

    void SetValueStatic(Variant& value) const override {
        if constexpr (!Traits::IsStatic) {
            throw std::logic_error("cannot set static value on a non-static type.");
        }
        else {
            *m_Field = value.GetValue<typename Traits::Type>();
        }
    }

    [[nodiscard]]
    Variant GetValue(const Variant& instance) const override {
        if constexpr (Traits::IsStatic) {
            return GetValueStatic();
        }
        else {
            return static_cast<typename Traits::Type>(instance.GetValue<typename Traits::ClassType>().*m_Field);
        }
    }

    void SetValue(const Variant& instance, const Variant& value) const override {
        if constexpr (Traits::IsStatic) {
            SetValueStatic(value);
        }
        else {
            static_cast<typename Traits::Type>(instance.GetValue<typename Traits::ClassType>().*m_Field) = value.GetValue<typename Traits::Type>();
        }
    }
};
}
