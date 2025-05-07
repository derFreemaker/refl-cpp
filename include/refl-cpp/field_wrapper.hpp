#pragma once

#include "common/unreachable.hpp"
#include "refl-cpp/common/type_traits.hpp"
#include "refl-cpp/variant.hpp"
#include "refl-cpp/declare_reflect.hpp"
#include "refl-cpp/field_traits.hpp"

namespace ReflCpp {
enum class FieldGetError : uint8_t {
    VariantIsVoid,
    InstanceIsVoid,
    CanNotGet,

    ReflectMaxLimitReached,
    ReflectCreationFailed,

    OutOfMemory,
};
}

template <>
struct ::rescpp::type_converter<ReflCpp::VariantGetError, ReflCpp::FieldGetError> {
    static ReflCpp::FieldGetError convert(const ReflCpp::VariantGetError& error) noexcept {
        switch (error) {
            case ReflCpp::VariantGetError::IsVoid:
                return ReflCpp::FieldGetError::VariantIsVoid;
            case ReflCpp::VariantGetError::CanNotGet:
                return ReflCpp::FieldGetError::CanNotGet;
        }
        ReflCpp::unreachable<true>();
    }
};

template <>
struct ::rescpp::type_converter<ReflCpp::ReflectError, ReflCpp::FieldGetError> {
    static ReflCpp::FieldGetError convert(const ReflCpp::ReflectError& error) noexcept {
        switch (error) {
            case ReflCpp::ReflectError::MaxLimitReached:
                return ReflCpp::FieldGetError::ReflectMaxLimitReached;
            case ReflCpp::ReflectError::CreationFailed:
                return ReflCpp::FieldGetError::ReflectCreationFailed;
            case ReflCpp::ReflectError::OutOfMemory:
                return ReflCpp::FieldGetError::OutOfMemory;
        }
        ReflCpp::unreachable<true>();
    }
};

namespace ReflCpp {
enum class FieldSetError : uint8_t {
    IsConst,
    IsNotCopyAssignable,
    InstanceIsVoid,

    NewValueIsVoid,
    CanNotGetNewValue,
};
}

template <>
struct ::rescpp::type_converter<ReflCpp::VariantGetError, ReflCpp::FieldSetError> {
    static ReflCpp::FieldSetError convert(const ReflCpp::VariantGetError& error) noexcept {
        switch (error) {
            case ReflCpp::VariantGetError::IsVoid:
                return ReflCpp::FieldSetError::NewValueIsVoid;
            case ReflCpp::VariantGetError::CanNotGet:
                return ReflCpp::FieldSetError::CanNotGetNewValue;
        }
        ReflCpp::unreachable<true>();
    }
};

namespace ReflCpp {
struct FieldBase {
    virtual ~FieldBase() = default;

    [[nodiscard]]
    virtual bool IsStatic() const = 0;

    [[nodiscard]]
    virtual bool IsConst() const = 0;

    [[nodiscard]]
    virtual rescpp::result<TypeID, ReflectError> GetType() const = 0;

    [[nodiscard]]
    virtual rescpp::result<Variant, FieldGetError> GetValue(const Variant& instance) const = 0;

    [[nodiscard]]
    virtual rescpp::result<void, FieldSetError> SetValue(const Variant& value, const Variant& instance) const = 0;

    [[nodiscard]]
    virtual rescpp::result<Variant, FieldGetError> GetRef(const Variant& instance) const = 0;
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
    rescpp::result<TypeID, ReflectError> GetType() const override {
        return ReflectID<typename Traits::Type>();
    }

    [[nodiscard]]
    rescpp::result<Variant, FieldGetError> GetValue(const Variant& instance) const noexcept override {
        if constexpr (Traits::IsStatic) {
            return Variant::Create<typename Traits::Type>(static_cast<typename Traits::Type>(*ptr_));
        }
        else {
            if (instance.IsVoid()) {
                return rescpp::fail(FieldGetError::InstanceIsVoid);
            }

            typename Traits::ClassType& obj = TRY(instance.Get<typename Traits::ClassType&>());
            return Variant::Create<typename Traits::Type>(static_cast<typename Traits::Type>(obj.*ptr_));
        }
    }

    [[nodiscard]]
    rescpp::result<void, FieldSetError> SetValue(const Variant& value, const Variant& instance) const noexcept override {
        if constexpr (Traits::IsConst) {
            return rescpp::fail(FieldSetError::IsConst);
        }
        else if constexpr (!std::is_copy_assignable_v<typename Traits::Type>) {
            //TODO: make it so that it only errors if there is no possible way
            return rescpp::fail(FieldSetError::IsNotCopyAssignable);
        }
        else if constexpr (Traits::IsStatic) {
            *ptr_ = TRY(value.Get<const typename Traits::Type&>());
            return {};
        }
        else {
            if (instance.IsVoid()) {
                rescpp::fail(FieldSetError::InstanceIsVoid);
            }

            typename Traits::ClassType& obj = TRY(instance.Get<typename Traits::ClassType&>());
            obj.*ptr_ = TRY(value.Get<make_lvalue_reference_t<make_const_t<typename Traits::Type>>>());
            return {};
        }
    }

    [[nodiscard]]
    rescpp::result<Variant, FieldGetError> GetRef(const Variant& instance) const noexcept override {
        using return_type = std::conditional_t<
            Traits::IsConst,
            make_lvalue_reference_t<const typename Traits::Type>,
            make_lvalue_reference_t<typename Traits::Type>
        >;

        if constexpr (Traits::IsStatic) {
            return Variant::Create<return_type>(static_cast<return_type>(*ptr_));
        }
        else {
            if (instance.IsVoid()) {
                return rescpp::fail(FieldGetError::InstanceIsVoid);
            }

            auto& obj = TRY(instance.Get<typename Traits::ClassType&>());
            return Variant::Create<return_type>(static_cast<return_type>(obj.*ptr_));
        }
    }
};
}
