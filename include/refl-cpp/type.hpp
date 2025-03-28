#pragma once

#include <vector>
#include <optional>
#include <ostream>
#include <sstream>
#include <algorithm>

#include "refl-cpp/declare_reflect.hpp"
#include "refl-cpp/type_data.hpp"
#include "refl-cpp/reflect_printer.hpp"
#include "refl-cpp/method.hpp"
#include "refl-cpp/field.hpp"
#include "refl-cpp/type_flags.hpp"

namespace ReflCpp {
struct Type {
private:
    const TypeID m_ID;

    const char* m_Name;
    std::optional<const char*> m_Namespace;
    const std::vector<TypeID> m_Bases;
    const std::vector<TypeID> m_Inners;

    const TypeFlags m_Flags;

    const std::vector<Method> m_Methods;
    const std::vector<Field> m_Fields;

    const ReflectPrintFunc m_PrintFunc;

public:
    Type() = delete;
    Type(const Type&) = delete;
    Type(const Type&&) = delete;

    Type(const TypeID id, const TypeData& data, const TypeOptions& options)
        : m_ID(id),

          m_Name(data.name),
          m_Namespace(data._namespace),
          m_Bases(data.bases),
          m_Inners(data.inners),

          m_Flags(data.flags),

          m_Methods(data.methods),
          m_Fields(data.fields),

          m_PrintFunc(options.print_func) {}

    [[nodiscard]]
    TypeID GetID() const {
        return m_ID;
    }

    [[nodiscard]]
    const char* GetName() const {
        return m_Name;
    }

    [[nodiscard]]
    const char* GetNamespace() const {
        if (m_Namespace.has_value()) {
            return m_Namespace.value();
        }
        return "";
    }

    [[nodiscard]]
    bool InNamespace() const {
        return m_Namespace.has_value();
    }

    [[nodiscard]]
    bool HasBases() const {
        return !m_Bases.empty();
    }
    
    [[nodiscard]]
    const std::vector<TypeID>& GetBases() const {
        return m_Bases;
    }

    [[nodiscard]]
    Result<const Type&> GetBase(const size_t index) const {
        return m_Bases[index].GetType();
    }
    
    [[nodiscard]]
    bool HasInners() const {
        return !m_Inners.empty();
    }

    [[nodiscard]]
    const std::vector<TypeID>& GetInners() const {
        return m_Inners;
    }

    [[nodiscard]]
    Result<const Type&> GetInner(const size_t index) const {
        return m_Inners[index].GetType();
    }

    [[nodiscard]]
    bool HasInner(const TypeID id) const {
        return std::ranges::any_of(m_Inners, [id](const TypeID t) {
            return t == id;
        });
    }

    [[nodiscard]]
    const TypeFlags& GetFlags() const {
        return m_Flags;
    }

    // fields

    [[nodiscard]]
    const std::vector<Field>& GetFields() const {
        return m_Fields;
    }

    std::optional<std::reference_wrapper<const Field>> GetField(const char* name) const {
        for (const auto& field : m_Fields) {
            if (field.GetName() == name) {
                return field;
            }
        }
        return std::nullopt;
    }

    // methods

    [[nodiscard]]
    const std::vector<Method>& GetMethods() const {
        return m_Methods;
    }

    std::optional<std::reference_wrapper<const Method>> GetMethod(const char* name) const {
        for (const auto& method : m_Methods) {
            if (method.GetName() == name) {
                return method;
            }
        }
        return std::nullopt;
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

    template <typename T>
    [[nodiscard]]
    bool Is() const {
        return Is(ReflectID<T>());
    }

    void Print(std::ostream& stream) const {
        if (m_PrintFunc != nullptr) {
            return m_PrintFunc(stream, *this);
        }

        if (InNamespace()) {
            stream << GetNamespace() << "::";
        }
        stream << GetName();
    }

    [[nodiscard]]
    std::string Dump() const {
        std::stringstream stream;
        if (m_PrintFunc != nullptr) {
            m_PrintFunc(stream, *this);
            return stream.str();
        }

        if (InNamespace()) {
            stream << GetNamespace() << "::";
        }
        stream << GetName();
        return stream.str();
    }
};
}

inline std::ostream& operator<<(std::ostream& stream, const ReflCpp::Type& type) {
    type.Print(stream);
    return stream;
}
