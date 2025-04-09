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

template <typename T_>
struct FieldWrapper final : public FieldBase {
private:
    T_ ptr_;

public:
    using Traits = FieldTraits<T_>;

    FieldWrapper(T_ ptr)
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
    Result<TypeID> GetType() const override {
        return ReflectID<typename Traits::Type>();
    }

    [[nodiscard]]
    Result<Variant> GetValue(const Variant& instance) const override {
        if constexpr (Traits::IsStatic) {
            return Variant::Create<make_lvalue_reference_t<typename Traits::Type>>(static_cast<make_lvalue_reference_t<typename Traits::Type>>(*ptr_));
        }
        else {
            if (instance.IsVoid()) {
                return { RESULT_ERROR(), "instance is needed for non-static member fields" };
            }

            typename Traits::ClassType& obj = TRY(instance.Get<typename Traits::ClassType&>());
            return Variant::Create<make_lvalue_reference_t<typename Traits::Type>>(static_cast<make_lvalue_reference_t<typename Traits::Type>>(obj.*ptr_));
        }
    }

    [[nodiscard]]
    Result<void> SetValue(const Variant& value, const Variant& instance) const override {
        if constexpr (Traits::IsConst) {
            const auto& type = TRY(Reflect<const typename Traits::Type>());
            return { RESULT_ERROR(), "cannot set value on type: {0}", type.Dump() };
        }
        if constexpr (!std::is_copy_constructible_v<typename Traits::Type> || !std::is_copy_assignable_v<typename Traits::Type>) {
            //TODO: make it so that it only errors if there is no possible way
            return { RESULT_ERROR(), "cannot set value on not copy construct or copy assignable" };
        }
        else if constexpr (Traits::IsStatic) {
            *ptr_ = value.Get<const typename Traits::Type&>();
        }
        else {
            if (instance.IsVoid()) {
                return { RESULT_ERROR(), "instance is needed for non-static member fields" };
            }

            typename Traits::ClassType& obj = TRY(instance.Get<typename Traits::ClassType&>());
            obj.*ptr_ = value.Get<const typename Traits::Type&>();

            return {};
        }

        return { RESULT_ERROR(), "unreachable" };
    }

    [[nodiscard]]
    Result<Variant> GetRef(const Variant& instance) const override {
        if constexpr (Traits::IsConst) {
            // Use 'GetValue', since it will use a const reference when it can.
            return { RESULT_ERROR(), "cannot get reference to a const type" };
        }
        else if constexpr (Traits::IsStatic) {
            return Variant::Create<make_lvalue_reference_t<typename Traits::Type>>(static_cast<typename Traits::Type&>(*ptr_));
        }
        else {
            if (instance.IsVoid()) {
                return { RESULT_ERROR(), "instance is needed for a non-static member field" };
            }

            auto& obj = TRY(instance.Get<typename Traits::ClassType&>());
            return Variant::Create<make_lvalue_reference_t<typename Traits::Type>>(static_cast<make_lvalue_reference_t<typename Traits::Type>>(obj.*ptr_));
        }
    }
};
}
