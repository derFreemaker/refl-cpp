#pragma once

#include "refl-cpp/variant_wrapper.hpp"

namespace ReflCpp {
struct Variant;

namespace detail {
template <typename T_>
concept LimitVariant = !std::is_same_v<T_, Variant>
    && !std::is_same_v<T_, std::nullptr_t>;
}

struct Variant {
private:
    std::shared_ptr<VariantBase> m_Base;

    TypeID m_Type;
    bool m_IsConst = false;

    Variant(std::nullptr_t)
        : m_Base(std::make_shared<VoidVariantWrapper>()),
          m_Type(ReflectID<void>()) {}

public:
    static Variant Void() {
        return Variant(nullptr);
    }

    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(T_& data)
        : m_Base(std::make_shared<RefVariantWrapper<T_>>(data)),
          m_Type(ReflectID<T_>()) {}

    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(const T_& data)
        : m_Base(std::make_shared<ValueVariantWrapper<T_>>(data)),
          m_Type(ReflectID<const T_>()),
          m_IsConst(true) {}

    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(T_* data)
        : m_Base(std::make_shared<PointerVariantWrapper<T_>>(data)),
          m_Type(ReflectID<T_*>()) {}

    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(const T_* data)
        : m_Base(std::make_shared<ConstPointerVariantWrapper<T_>>(data)),
          m_Type(ReflectID<const T_*>()),
          m_IsConst(true) {}

    [[nodiscard]]
    TypeID GetType() const {
        return m_Type;
    }

    template <typename T_>
        requires detail::LimitVariant<T_>
    [[nodiscard]]
    T_& GetRef() const {
        if (m_Type == ReflectID<void>()) {
            throw std::logic_error("cannot get reference to void variant");
        }
        
        if (m_Type != ReflectID<T_>()) {
            throw std::invalid_argument("passed type is not the same as the stored type");
        }

        if (m_IsConst) {
            throw std::logic_error("cannot get reference to constant");
        }

        return static_cast<VariantWrapper<T_&>*>(m_Base.get())->GetValue();
    }

    template <typename T_>
        requires detail::LimitVariant<T_>
    [[nodiscard]]
    const T_& GetValue() const;
};
}
