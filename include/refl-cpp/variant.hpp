#pragma once

#include "refl-cpp/common/result.hpp"
#include "refl-cpp/variant_wrapper.hpp"

namespace ReflCpp {
struct Variant;

namespace detail {
template <typename T_>
concept LimitVariant = !std::is_same_v<T_, Variant>
    && !std::is_same_v<T_, std::nullptr_t>;
}

//TODO: decide if we want variant to be copy able
struct Variant {
private:
    std::shared_ptr<VariantBase> m_Base;

    TypeID m_Type;
    bool m_IsConst = false;

    Variant(std::nullptr_t)
        : m_Base(std::make_shared<VoidVariantWrapper>()),
          m_Type(ReflectID<void>().Value()) {}

    void CheckVoid() const {
        if (IsVoid()) {
            throw std::logic_error("cannot get reference to void variant");
        }
    }

    friend struct VariantTestHelper;

public:
    static Variant Void() {
        static Variant instance(nullptr);
        return instance;
    }

    template <typename T_>
        requires (detail::LimitVariant<T_> && std::copy_constructible<T_>)
    Variant(const T_& data)
        : m_Base(std::make_shared<ValueVariantWrapper<T_>>(data)),
          m_Type(ReflectID<const T_>().Value()),
          m_IsConst(true) {}
    
    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(T_& data)
        : m_Base(std::make_shared<RefVariantWrapper<T_>>(data)),
          m_Type(ReflectID<T_>().Value()) {}

    template <typename T_>
        requires (detail::LimitVariant<T_> && !std::copy_constructible<T_>)
    Variant(const T_& data)
        : m_Base(std::make_shared<ConstRefVariantWrapper<T_>>(data)),
          m_Type(ReflectID<const T_>().Value()),
          m_IsConst(true) {}

    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(T_* data)
        : m_Base(std::make_shared<PointerVariantWrapper<T_>>(data)),
          m_Type(ReflectID<T_*>().Value()) {}

    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(const T_* data)
        : m_Base(std::make_shared<ConstPointerVariantWrapper<T_>>(data)),
          m_Type(ReflectID<const T_*>().Value()),
          m_IsConst(true) {}

    [[nodiscard]]
    bool IsVoid() const {
        return m_Type == ReflectID<void>().Value();
    }

    [[nodiscard]]
    TypeID GetType() const {
        return m_Type;
    }

    //TODO: add get const reference or something
    
    template <typename T_>
        requires detail::LimitVariant<T_>
    [[nodiscard]]
    Result<T_&> GetRef() const;

    template <typename T_>
        requires (detail::LimitVariant<T_> && !std::is_pointer_v<T_>)
    [[nodiscard]]
    Result<const T_&> GetValue() const;

    template <typename T_>
        requires (detail::LimitVariant<T_> && std::is_pointer_v<T_>)
    [[nodiscard]]
    Result<const std::remove_pointer_t<T_>*&> GetValue() const;
};
}
