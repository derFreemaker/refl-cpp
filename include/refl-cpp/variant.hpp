#pragma once

#include "refl-cpp/variant_wrapper.hpp"

namespace ReflCpp {
struct Variant {
private:
    VariantBase* m_Base;

public:
    static Variant Void() {
        return Variant(nullptr);
    }

    template <typename T_>
        requires (sizeof(T_) <= 8)
    Variant(T_ data)
        : m_Base(new VariantWrapper<T_>(data)) {}

    template <typename T_>
        requires (sizeof(T_) > 8)
    Variant(T_& data)
        : m_Base(new VariantWrapper<T_>(data)) {}

    ~Variant() {
        delete m_Base;
    }

    [[nodiscard]]
    TypeID GetType() const {
        return m_Base->GetType();
    }

    template <typename T_>
    T_ GetValue() const {
        if (m_Base->GetType() == ReflectID<T_>()) {
            throw std::invalid_argument("passed type is not the same as the stored type");
        }

        return dynamic_cast<VariantWrapper<T_>*>(m_Base)->GetValue();
    }
};
}
