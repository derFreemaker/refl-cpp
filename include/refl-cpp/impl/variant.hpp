#pragma once

#include "refl-cpp/variant.hpp"

#include "refl-cpp/impl/variant_wrapper.hpp"
#include "refl-cpp/impl/variant_matcher.hpp"
#include "refl-cpp/reflect.hpp"

namespace ReflCpp {
inline Variant& Variant::Void() noexcept {
    static detail::VoidVariantWrapper wrapper;
    static Variant instance(
        std::shared_ptr<detail::VoidVariantWrapper>(&wrapper, [](detail::VoidVariantWrapper*) {}),
        ReflectionDatabase::Void().GetID()
    );
    return instance;
}

template <typename T>
    requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
rescpp::result<Variant, ReflectError> Variant::Create(T& data) {
    return Variant(detail::MakeWrapper<T>(std::forward<T>(data)), TRY(ReflectID<T>()));
}

template <typename T>
    requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
rescpp::result<Variant, ReflectError> Variant::Create(T&& data) {
    return Variant(detail::MakeWrapper<T>(std::forward<T>(data)), TRY(ReflectID<T>()));
}

template <typename T>
    requires (std::is_reference_v<T>)
rescpp::result<Variant, ReflectError> Variant::Create(T&& data) {
    return Variant(detail::MakeWrapper<T>(std::forward<T>(data)), TRY(ReflectID<T>()));
}

template <typename T>
    requires (std::is_pointer_v<T>)
rescpp::result<Variant, ReflectError> Variant::Create(T data) {
    return Variant(detail::MakeWrapper<T>(std::forward<T>(data)), TRY(ReflectID<T>()));
}

template <typename T>
bool Variant::CanGet() const noexcept {
#define REFLCPP_VARIANT_MATCH(TYPE) \
    case detail::VariantWrapperType::TYPE: { \
        if (detail::VariantMatcher<detail::VariantWrapperType::TYPE, T>::Match(type_)) { \
            return true; \
        } \
        break; \
    }

    switch (base_->GetType()) {
        REFLCPP_VARIANT_MATCH(VOID)
        REFLCPP_VARIANT_MATCH(VALUE)
        REFLCPP_VARIANT_MATCH(CONST_VALUE)
        REFLCPP_VARIANT_MATCH(LVALUE_REF)
        REFLCPP_VARIANT_MATCH(CONST_LVALUE_REF)
        REFLCPP_VARIANT_MATCH(RVALUE_REF)
        REFLCPP_VARIANT_MATCH(CONST_RVALUE_REF)
        REFLCPP_VARIANT_MATCH(POINTER)
        REFLCPP_VARIANT_MATCH(CONST_POINTER)

        default:
            break;
    }

#undef REFLCPP_VARIANT_MATCH

    return false;
}

template <typename T>
rescpp::result<void, VariantGetError> Variant::CheckGet() const noexcept {
    if (CanGet<T>()) {
        return {};
    }
    return rescpp::fail(VariantGetError::CanNotGet);
}

namespace detail {
template <VariantWrapperType Type, typename R>
concept HasVariantMatcher = requires(VariantBase* base) {
    { VariantMatcher<Type, R>::Get(base) };
};
}

template <typename ReturnT, typename T>
rescpp::result<ReturnT, VariantGetError> Variant::GetImpl() const {
    TRY(CheckVoid());
    TRY(CheckGet<T>());

#define REFLCPP_MATCH_VARIANT(Type) \
    if constexpr (detail::HasVariantMatcher<detail::VariantWrapperType::Type, T>) { \
        if (wrapperType == detail::VariantWrapperType::Type) { \
            return detail::VariantMatcher<detail::VariantWrapperType::Type, T>::Get(base_.get()); \
        } \
    }

    const auto wrapperType = base_->GetType();
    REFLCPP_MATCH_VARIANT(VALUE)
    REFLCPP_MATCH_VARIANT(CONST_VALUE)
    REFLCPP_MATCH_VARIANT(LVALUE_REF)
    REFLCPP_MATCH_VARIANT(CONST_LVALUE_REF)
    REFLCPP_MATCH_VARIANT(RVALUE_REF)
    REFLCPP_MATCH_VARIANT(CONST_RVALUE_REF)
    REFLCPP_MATCH_VARIANT(POINTER)
    REFLCPP_MATCH_VARIANT(CONST_POINTER)

#undef REFLCPP_MATCH_VARIANT
    unreachable<true>();
}

template <typename T>
    requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
rescpp::result<std::remove_volatile_t<T>&, VariantGetError> Variant::Get() const {
    return GetImpl<std::remove_volatile_t<T>&, T>();
}

template <typename T>
    requires (std::is_lvalue_reference_v<T>)
rescpp::result<std::remove_volatile_t<std::remove_reference_t<T>>&, VariantGetError> Variant::Get() const {
    return GetImpl<std::remove_volatile_t<std::remove_reference_t<T>>&, T>();
}

template <typename T>
    requires (std::is_rvalue_reference_v<T>)
rescpp::result<std::remove_volatile_t<std::remove_reference_t<T>>&&, VariantGetError> Variant::Get() const {
    return GetImpl<std::remove_volatile_t<std::remove_reference_t<T>>&&, T>();
}

template <typename T>
    requires (std::is_pointer_v<T>)
rescpp::result<std::remove_volatile_t<std::remove_pointer_t<T>>*, VariantGetError> Variant::Get() const noexcept {
    return GetImpl<std::remove_volatile_t<std::remove_pointer_t<T>>*, T>();
}
}
