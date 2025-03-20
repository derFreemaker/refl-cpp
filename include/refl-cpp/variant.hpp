#pragma once

#include "refl-cpp/common/result.hpp"
#include "refl-cpp/type_id.hpp"

namespace ReflCpp {
struct VariantBase {
    virtual ~VariantBase() = default;
};

template <typename R_>
struct VariantWrapper : public VariantBase {
    [[nodiscard]]
    virtual R_ GetValue() = 0;
};

struct VoidVariantWrapper final : public VariantWrapper<void> {
    void GetValue() override {}
};

template <typename T_>
struct ValueVariantWrapper final : public VariantWrapper<const T_&> {
private:
    const T_ m_Value;

public:
    ValueVariantWrapper(const T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct RefVariantWrapper final : public VariantWrapper<T_&> {
private:
    T_& m_Value;

public:
    RefVariantWrapper(T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    T_& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct ConstRefVariantWrapper final : public VariantWrapper<const T_&> {
private:
    const T_& m_Value;

public:
    ConstRefVariantWrapper(const T_& value)
        : m_Value(value) {}


    [[nodiscard]]
    const T_& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct PointerVariantWrapper final : public VariantWrapper<T_*&> {
private:
    T_* m_Value;

public:
    PointerVariantWrapper(T_* value)
        : m_Value(value) {}

    [[nodiscard]]
    T_*& GetValue() override {
        return m_Value;
    }
};

template <typename T_>
struct ConstPointerVariantWrapper final : public VariantWrapper<const T_*&> {
private:
    const T_* m_Value;

public:
    ConstPointerVariantWrapper(const T_* value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_*& GetValue() override {
        return m_Value;
    }
};

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

    [[nodiscard]]
    Result<void> CheckVoid() const {
        if (IsVoid()) {
            return { Error, "cannot get reference or value from void variant" };
        }
        return { Ok };
    }

    Variant()
        : m_Base(std::make_shared<VoidVariantWrapper>()),
          m_Type(*ReflectID<void>()) {}

    friend struct VariantTestHelper;

public:
    static Variant Void() {
        static auto instance = Variant();
        return instance;
    }

    Variant(std::nullptr_t)
        : m_Base(std::make_shared<PointerVariantWrapper<std::nullptr_t>>(nullptr)),
          m_Type(*ReflectID<std::nullptr_t>()) {}

    template <typename T_>
        requires (detail::LimitVariant<T_> && std::copy_constructible<T_>)
    Variant(const T_& data)
        : m_Base(std::make_shared<ValueVariantWrapper<T_>>(data)),
          m_Type(*ReflectID<const T_>()),
          m_IsConst(true) {}

    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(T_& data)
        : m_Base(std::make_shared<RefVariantWrapper<T_>>(data)),
          m_Type(*ReflectID<T_>()) {}

    template <typename T_>
        requires (detail::LimitVariant<T_> && !std::copy_constructible<T_>)
    Variant(const T_& data)
        : m_Base(std::make_shared<ConstRefVariantWrapper<T_>>(data)),
          m_Type(*ReflectID<const T_>()),
          m_IsConst(true) {}

    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(T_* data)
        : m_Base(std::make_shared<PointerVariantWrapper<T_>>(data)),
          m_Type(*ReflectID<T_*>()) {}

    template <typename T_>
        requires detail::LimitVariant<T_>
    Variant(const T_* data)
        : m_Base(std::make_shared<ConstPointerVariantWrapper<T_>>(data)),
          m_Type(*ReflectID<const T_*>()),
          m_IsConst(true) {}

    [[nodiscard]]
    bool IsVoid() const {
        return m_Type == *ReflectID<void>();
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
    Result<std::add_const_t<std::remove_pointer_t<T_>>*&> GetValue() const;
};
}
