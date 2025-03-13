#pragma once

#include <vector>

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/variant.hpp"

namespace ReflCpp {
struct Type;

struct ArgumentInfo {
    const char* name;
    const TypeID type;
};

struct ArgumentList {
private:
    std::vector<Variant> m_Data;

public:
    template <typename... Args>
    ArgumentList(Args&&... args)
        : m_Data({Variant(std::forward<Args>(args))...}) {}

    [[nodiscard]]
    size_t Size() const {
        return m_Data.size();
    }

    const Variant& operator[](const size_t index) const {
        return m_Data[index];
    }
};
}
