#pragma once

#include "refl-cpp/variant.hpp"

#include "refl-cpp/impl/variant_wrapper.hpp"
#include "refl-cpp/impl/variant_matcher.hpp"
#include "refl-cpp/reflect.hpp"

namespace ReflCpp {
inline void Variant::ThrowCanNotGetFromVariantWithType(const Type& type, const Type& passed_type) {
    throw std::logic_error(
        "cannot get from Variant (" + type.Dump() + ")"
        " with passed type: {1}" + passed_type.Dump()
    );
}

inline Variant& Variant::Void() {
    // static detail::VoidVariantWrapper wrapper;
    static Variant instance(
        std::make_shared<detail::VoidVariantWrapper>(),
        ReflectID<void>()
    );
    return instance;
}

template <typename T>
    requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
Variant Variant::Create(T& data) {
    return Variant(detail::MakeWrapper<T>(std::forward<T>(data)), ReflectID<T>());
}

template <typename T>
    requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
Variant Variant::Create(T&& data) {
    return Variant(detail::MakeWrapper<T>(std::forward<T>(data)), ReflectID<T>());
}

template <typename T>
    requires (std::is_reference_v<T>)
Variant Variant::Create(T&& data) {
    return Variant(detail::MakeWrapper<T>(std::forward<T>(data)), ReflectID<T>());
}

template <typename T>
    requires (std::is_pointer_v<T>)
Variant Variant::Create(T data) {
    return Variant(detail::MakeWrapper<T>(std::forward<T>(data)), ReflectID<T>());
}

template <typename T>
bool Variant::CanGet() const {
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
void Variant::CheckGet() const {
    if (CanGet<T>()) {
        return;
    }

    const Type& type = type_.GetType();
    const Type& passed_type = Reflect<T>();
    ThrowCanNotGetFromVariantWithType(type, passed_type);
}

namespace detail {
template <VariantWrapperType Type, typename R>
concept HasVariantMatcher = requires(VariantBase* base) {
    { VariantMatcher<Type, R>::Get(base) };
};
}

template <typename ReturnT, typename T>
ReturnT Variant::GetImpl() const {
    CheckVoid();
    CheckGet<T>();

#define REFLCPP_MATCH_VARIANT(Type) \
    if constexpr (detail::HasVariantMatcher<detail::VariantWrapperType::Type, T>) { \
        if (wrapperType == detail::VariantWrapperType::Type) { \
            return detail::VariantMatcher<detail::VariantWrapperType::Type, T>::Get(base_.get()); \
        } \
    }

#define REFLCPP_MATCH_VARIANTS() \
    const auto wrapperType = base_->GetType(); \
    REFLCPP_MATCH_VARIANT(VALUE) \
    REFLCPP_MATCH_VARIANT(CONST_VALUE) \
    REFLCPP_MATCH_VARIANT(LVALUE_REF) \
    REFLCPP_MATCH_VARIANT(CONST_LVALUE_REF) \
    REFLCPP_MATCH_VARIANT(RVALUE_REF) \
    REFLCPP_MATCH_VARIANT(CONST_RVALUE_REF) \
    REFLCPP_MATCH_VARIANT(POINTER) \
    REFLCPP_MATCH_VARIANT(CONST_POINTER) \
    throw std::logic_error("unreachable");


    REFLCPP_MATCH_VARIANTS()

#undef REFLCPP_MATCH_VARIANTS
#undef REFLCPP_MATCH_VARIANT
}

template <typename T>
    requires (!std::is_reference_v<T> && !std::is_pointer_v<T>)
std::remove_volatile_t<T>& Variant::Get() const {
    return GetImpl<std::remove_volatile_t<T>&, T>();
}

template <typename T>
    requires (std::is_lvalue_reference_v<T>)
std::remove_volatile_t<std::remove_reference_t<T>>& Variant::Get() const {
    return GetImpl<std::remove_volatile_t<std::remove_reference_t<T>>&, T>();
}

template <typename T>
    requires (std::is_rvalue_reference_v<T>)
std::remove_volatile_t<std::remove_reference_t<T>>&& Variant::Get() const {
    return GetImpl<std::remove_volatile_t<std::remove_reference_t<T>>&&, T>();
}

template <typename T>
    requires (std::is_pointer_v<T>)
std::remove_volatile_t<std::remove_pointer_t<T>>* Variant::Get() const noexcept {
    return GetImpl<std::remove_volatile_t<std::remove_pointer_t<T>>*, T>();
}
}
