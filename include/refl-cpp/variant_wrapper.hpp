#pragma once

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct VariantBase {
    virtual ~VariantBase() = default;
};

template <typename R_>
struct VariantWrapper : public VariantBase {
    [[nodiscard]]
    virtual R_ GetValue() const = 0;
};

struct VoidVariantWrapper final : public VariantWrapper<void> {
    void GetValue() const override {}
};

template <typename T_>
struct ValueVariantWrapper final : public VariantWrapper<const T_&> {
private:
    const T_ m_Value;

public:
    ValueVariantWrapper(const T_& value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_& GetValue() const override {
        return m_Value;
    }
};

template <typename T_>
    requires (!std::is_const_v<T_>)
struct RefVariantWrapper final : public VariantWrapper<T_&> {
private:
    T_& m_Value;

public:
    RefVariantWrapper(T_& value)
        : m_Value(value) {}
    

    [[nodiscard]]
    T_& GetValue() const override {
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
    T_*& GetValue() const override {
        return m_Value;
    }
};

template <typename T_>
struct ConstPointerVariantWrapper final : public VariantWrapper<const T_* const&> {
private:
    const T_* m_Value;

public:
    ConstPointerVariantWrapper(const T_* value)
        : m_Value(value) {}

    [[nodiscard]]
    const T_* const& GetValue() const override {
        return m_Value;
    }
};
}
