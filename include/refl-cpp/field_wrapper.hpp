#pragma once

#include "refl-cpp/common/type_traits.hpp"
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
    virtual Variant GetValue(const Variant& instance) const = 0;

    virtual void SetValue(const Variant& value, const Variant& instance) const = 0;

    [[nodiscard]]
    virtual Variant GetRef(const Variant& instance) const = 0;
};

template <typename T>
struct FieldWrapper final : public FieldBase {
private:
    T ptr_;

public:
    using Traits = FieldTraits<T>;

    FieldWrapper(T ptr) noexcept
        : ptr_(ptr) {}

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
    Variant GetValue(const Variant& instance) const noexcept override {
        if constexpr (Traits::IsStatic) {
            return Variant::Create<make_lvalue_reference_t<typename Traits::Type>>(static_cast<make_lvalue_reference_t<typename Traits::Type>>(*ptr_));
        }
        else {
            if (instance.IsVoid()) {
                throw std::logic_error("instance is needed for non-static member fields");
            }

            typename Traits::ClassType& obj = instance.Get<typename Traits::ClassType&>();
            return Variant::Create<make_lvalue_reference_t<typename Traits::Type>>(static_cast<make_lvalue_reference_t<typename Traits::Type>>(obj.*ptr_));
        }
    }

    void SetValue(const Variant& value, const Variant& instance) const noexcept override {
        const auto& type = Reflect<const typename Traits::Type>();
        if constexpr (Traits::IsConst) {
            throw std::logic_error("cannot set value on type: " + type.Dump());
        }
        if constexpr (!std::is_copy_constructible_v<typename Traits::Type> || !std::is_copy_assignable_v<typename Traits::Type>) {
            //TODO: make it so that it only errors if there is no possible way
            throw std::logic_error("cannot set value on not copy construct or copy assignable: " + type.Dump());
        }
        else if constexpr (Traits::IsStatic) {
            *ptr_ = value.Get<const typename Traits::Type&>();
        }
        else {
            if (instance.IsVoid()) {
                throw std::logic_error("instance is needed for non-static member fields");
            }

            typename Traits::ClassType& obj = instance.Get<typename Traits::ClassType&>();
            obj.*ptr_ = value.Get<make_lvalue_reference_t<make_const_t<typename Traits::Type>>>();
        }
    }

    //TODO: make one function for getting value or reference
    [[nodiscard]]
    Variant GetRef(const Variant& instance) const noexcept override {
        if constexpr (Traits::IsConst) {
            // Use 'GetValue', since it will use a const reference when it can.
            throw std::logic_error("cannot get reference to a const type");
        }
        else if constexpr (Traits::IsStatic) {
            return Variant::Create<make_lvalue_reference_t<typename Traits::Type>>(
                static_cast<make_lvalue_reference_t<typename Traits::Type>>(*ptr_)
            );
        }
        else {
            if (instance.IsVoid()) {
                throw std::logic_error("instance is needed for a non-static member field");
            }

            auto& obj = instance.Get<typename Traits::ClassType&>();
            return Variant::Create<make_lvalue_reference_t<typename Traits::Type>>(
                static_cast<make_lvalue_reference_t<typename Traits::Type>>(obj.*ptr_)
            );
        }
    }
};
}
