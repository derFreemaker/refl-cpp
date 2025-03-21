#pragma once

#include "refl-cpp/common/result.hpp"
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
    virtual Result<TypeID> GetType() const = 0;

    [[nodiscard]]
    virtual Result<Variant> GetValue(const Variant& instance) const = 0;

    [[nodiscard]]
    virtual Result<void> SetValue(const Variant& value, const Variant& instance) const = 0;

    [[nodiscard]]
    virtual Result<Variant> GetRef(const Variant& instance) const = 0;
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
    Result<TypeID> GetType() const override {
        return ReflectID<typename Traits::Type>();
    }
    
    [[nodiscard]]
    Result<Variant> GetValue(const Variant& instance) const override {
        if constexpr (Traits::IsStatic) {
            return { Ok, static_cast<typename Traits::Type>(*m_Field) };
        }
        else {
            if (instance.IsVoid()) {
                return { Error, "instance is needed for non-static member fields" };
            }
            
            typename Traits::Type& obj = TRY(instance.GetRef<typename Traits::ClassType>());
            return { Ok, static_cast<typename Traits::Type>(obj.*m_Field) };
        }
    }

    [[nodiscard]]
    Result<void> SetValue(const Variant& value, const Variant& instance) const override {
        if constexpr (Traits::IsConst) {
            return { Error, "cannot set value on a const type: {0}", TRY(Reflect<typename Traits::Type>()).Dump() };
        }
        else if constexpr (Traits::IsStatic) {
            *m_Field = value.GetValue<typename Traits::Type>();
        }
        else {
            if (instance.IsVoid()) {
                return { Error, "instance is needed for non-static member fields" };
            }
            
            typename Traits::ClassType& obj = TRY(instance.GetRef<typename Traits::ClassType>());
            obj.*m_Field = value.GetValue<typename Traits::Type>();

            return { Ok };
        }

        return { Error, "unreachable" };
    }

    [[nodiscard]]
    Result<Variant> GetRef(const Variant& instance) const override {
        if constexpr (Traits::IsConst) {
            // Use 'GetValue', since it will use a const reference when it can.
            return { Error, "cannot get reference to a const type" };
        }
        else if constexpr (Traits::IsStatic) {
            return { Ok, static_cast<typename Traits::Type&>(*m_Field) };
        }
        else {
            if (instance.IsVoid()) {
                return { Error, "instance is needed for a non-static member field" };
            }
            
            auto& obj = TRY(instance.GetRef<typename Traits::ClassType>());
            return { Ok, static_cast<typename Traits::Type&>(obj.*m_Field) };
        }
    }
};
}
