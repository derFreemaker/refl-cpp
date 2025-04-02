#pragma once

#include "refl-cpp/variant.hpp"

#include "refl-cpp/impl/variant_wrapper.hpp"
#include "refl-cpp/impl/variant_matcher.hpp"
#include "refl-cpp/reflect.hpp"

namespace ReflCpp {
template <typename T_>
Variant Variant::Create(T_&& data) {
    constexpr bool isConst = std::is_const_v<T_>;
    return Variant(detail::MakeWrapper<T_>(std::forward<T_>(data)), ReflectID<T_>().Value(), isConst);
}

inline FormattedError Variant::CanNotGetFromVariantWithType(const Type& type, const Type& passed_type) {
    return FormattedError {
        "cannot get from Variant ({0}) with passed type: {1}",
        type.Dump(),
        passed_type.Dump()
    };
}

inline Variant& Variant::Void() {
    static auto instance = Variant(
        std::make_shared<detail::VoidVariantWrapper>(),
        ReflectID<void>().Value(),
        false
    );
    return instance;
}

template <typename T_>
bool Variant::CanGet() const {
#define REFLCPP_VARIANT_MATCH(TYPE) \
    case detail::VariantWrapperType::TYPE: { \
        if (detail::VariantMatcher<detail::VariantWrapperType::TYPE, T_>::Match(m_Type)) { \
            return true; \
        } \
        break; \
    }

    switch (m_Base->GetType()) {
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

template <typename T_>
Result<void> Variant::CanGetWithError() const {
    if (CanGet<T_>()) {
        return {};
    }

    const Type& type = TRY(m_Type.GetType());
    const Type& passed_type = TRY(Reflect<T_>());
    return { RESULT_ERROR(), CanNotGetFromVariantWithType(type, passed_type) };
}

#define REFLCPP_MATCH_VARIANT(Type) \
    if constexpr (detail::HasVariantMatcher<detail::VariantWrapperType::Type, T_>) { \
        if (wrapperType == detail::VariantWrapperType::Type) { \
            return detail::VariantMatcher<detail::VariantWrapperType::Type, T_>::Get(m_Base.get()); \
        } \
    }

#define REFLCPP_MATCH_VARIANTS() \
    const auto wrapperType = m_Base->GetType(); \
    REFLCPP_MATCH_VARIANT(VALUE) \
    REFLCPP_MATCH_VARIANT(CONST_VALUE) \
    REFLCPP_MATCH_VARIANT(LVALUE_REF) \
    REFLCPP_MATCH_VARIANT(CONST_LVALUE_REF) \
    REFLCPP_MATCH_VARIANT(RVALUE_REF) \
    REFLCPP_MATCH_VARIANT(CONST_RVALUE_REF) \
    REFLCPP_MATCH_VARIANT(POINTER) \
    REFLCPP_MATCH_VARIANT(CONST_POINTER) \
    return { RESULT_ERROR(), "unreachable" };

template <typename T_>
    requires (!std::is_reference_v<T_> && !std::is_pointer_v<T_>)
Result<std::remove_volatile_t<T_>&> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    REFLCPP_MATCH_VARIANTS()
}

template <typename T_>
    requires (std::is_lvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    REFLCPP_MATCH_VARIANTS()
}

template <typename T_>
    requires (std::is_rvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    REFLCPP_MATCH_VARIANTS()
}

template <typename T_>
    requires (std::is_pointer_v<T_>)
Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    REFLCPP_MATCH_VARIANTS()
}

#undef REFLCPP_MATCH_VARIANTS
#undef REFLCPP_MATCH_VARIANT
}
