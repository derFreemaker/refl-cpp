#pragma once

#include "refl-cpp/variant_wrapper.hpp"

namespace ReflCpp {
struct Variant {
private:
    VariantBase* const m_Base;

public:
    static Variant Void() {
        return Variant(nullptr);
    }
    
    template <typename T_>
    Variant(T_ data)
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
        return std::any_cast<T_>(m_Base->GetValue());
    }
};
}
