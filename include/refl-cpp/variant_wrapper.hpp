#pragma once

#include <stdexcept>
#include <any>
#include <type_traits>

#include "refl-cpp/variant_traits.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct VariantBase {
    virtual ~VariantBase() = default;

    [[nodiscard]]
    virtual TypeID GetType() const = 0;

    [[nodiscard]]
    virtual std::any GetValue() const = 0;
};

template <typename T_>
struct VariantWrapper final : public VariantBase {
private:
    T_ m_Value;
    using Traits = VariantTraits<T_>;

public:
    VariantWrapper(T_ value)
        : m_Value(value) {}
    
    [[nodiscard]]
    TypeID GetType() const override {
        return ReflectID<typename Traits::Type>();
    }

    [[nodiscard]]
    std::any GetValue() const override {
        return m_Value;
    }
};
}