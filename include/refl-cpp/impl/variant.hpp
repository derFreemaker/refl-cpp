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

template <typename T_>
    requires (!std::is_reference_v<T_> && !std::is_pointer_v<T_>)
Result<std::remove_volatile_t<T_>&> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    const auto wrapperType = m_Base->GetType();

    // find better way for this:
    
    if (wrapperType == detail::VariantWrapperType::VALUE) {
        return detail::VariantMatcher<detail::VariantWrapperType::VALUE, T_>::Get(m_Base.get());
    }

    if (wrapperType == detail::VariantWrapperType::LVALUE_REF) {
        return detail::VariantMatcher<detail::VariantWrapperType::LVALUE_REF, T_>::Get(m_Base.get());
    }
    
    if constexpr (std::is_const_v<T_>) {
        if (wrapperType == detail::VariantWrapperType::CONST_VALUE) {
            return detail::VariantMatcher<detail::VariantWrapperType::CONST_VALUE, T_>::Get(m_Base.get());
        }
        // if (wrapperType == detail::VariantWrapperType::CONST_LVALUE_REF) {
        //     return detail::VariantMatcher<detail::VariantWrapperType::CONST_LVALUE_REF, T_>::Get(m_Base.get());
        // }
    }
    
    return { RESULT_ERROR(), "unreachable" };
}

template <typename T_>
    requires (std::is_lvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    return static_cast<detail::VariantWrapper<std::remove_volatile_t<std::remove_reference_t<T_>>&>*>(m_Base.get())->GetValue();
}

template <typename T_>
    requires (std::is_rvalue_reference_v<T_>)
Result<std::remove_volatile_t<std::remove_reference_t<T_>>&&> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    using ResultT = std::remove_volatile_t<std::remove_reference_t<T_>>&&;
    return std::forward<ResultT>(static_cast<detail::VariantWrapper<ResultT>*>(m_Base.get())->GetValue());
}

template <typename T_>
    requires (std::is_pointer_v<T_>)
Result<std::remove_volatile_t<std::remove_pointer_t<T_>>*> Variant::Get() const {
    TRY(CheckVoid());
    TRY(CanGetWithError<T_>());

    return static_cast<detail::VariantWrapper<std::remove_volatile_t<std::remove_pointer_t<T_>>*>*>(m_Base.get())->GetValue();
}
}
