#pragma once

#include <vector>
#include <optional>
#include <ostream>

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/reflect_data.hpp"

namespace ReflCpp {

struct Type;
using ReflectPrintFunc = std::function<void(std::ostream& stream, const Type& type)>;

template <typename T>
concept has_reflect_printer = requires(std::ostream& stream, const Type& type) {
    { ReflectData<T>::Print(stream, type) } -> std::same_as<void>;
};

struct Type {
    Type() = delete;
    Type(Type&) = delete;
    Type(Type&&) = delete;

    Type(const TypeID id, const ReflectTypeData& data, const ReflectPrintFunc& print_func)
        : m_ID(id),

          m_Name(data.name),
          m_Bases(data.bases),
          m_Inner(data.inner),

          m_IsArray(data.is_array),
          m_IsPointer(data.is_pointer),
          m_IsReference(data.is_reference),
          m_IsConst(data.is_const),
          m_IsVolatile(data.is_volatile),

          m_PrintFunc(print_func) {}

    [[nodiscard]]
    TypeID GetID() const {
        return m_ID;
    }

    operator TypeID() const {
        return m_ID;
    }

    [[nodiscard]]
    const char* GetName() const {
        return m_Name;
    }

    operator const char*() const {
        return m_Name;
    }

    [[nodiscard]]
    const std::vector<TypeID>& GetBases() const {
        return m_Bases;
    }

    [[nodiscard]]
    bool HasInner() const {
        return m_Inner.has_value();
    }

    [[nodiscard]]
    const Type& GetInner() const;

    // flags

    [[nodiscard]]
    bool IsArray() const {
        return m_IsArray;
    }

    [[nodiscard]]
    bool IsPointer() const {
        return m_IsPointer;
    }

    [[nodiscard]]
    bool IsReference() const {
        return m_IsReference;
    }

    [[nodiscard]]
    bool IsConst() const {
        return m_IsConst;
    }

    [[nodiscard]]
    bool IsVolatile() const {
        return m_IsVolatile;
    }

    // utils

    [[nodiscard]]
    bool Is(const TypeID id) const {
        return m_ID == id;
    }

    [[nodiscard]]
    bool Is(const Type& type) const {
        return Is(type.GetID());
    }

    void Print(std::ostream& stream) const {
        if (m_PrintFunc != nullptr) {
            return m_PrintFunc(stream, *this);
        }

        stream << GetName();
    }

private:
    const TypeID m_ID;

    const char* m_Name;
    const std::vector<TypeID> m_Bases;
    const std::optional<TypeID> m_Inner;

    const bool m_IsArray;
    const bool m_IsPointer;
    const bool m_IsReference;
    const bool m_IsConst;
    const bool m_IsVolatile;

    const ReflectPrintFunc m_PrintFunc;
};
}
