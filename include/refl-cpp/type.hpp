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
    const TypeID id_;

    const char* name_;
    std::optional<const char*> namespace_;
    const std::vector<TypeID> bases_;
    const std::vector<TypeID> inners_;

    const TypeFlags flags_;

    const std::vector<Method> methods_;
    const std::vector<Field> fields_;

    const ReflectPrintFunc printFunc_;

public:
    Type() = delete;
    Type(const Type&) = delete;
    Type(const Type&&) = delete;

    Type(const TypeID id, const TypeData& data, const TypeOptions& options)
        : id_(id),

          name_(data.name),
          namespace_(data._namespace),
          bases_(data.bases),
          inners_(data.inners),

          flags_(data.flags),

          // m_Methods(data.methods),
          fields_(data.fields),

          printFunc_(options.printFunc) {}

    [[nodiscard]]
    TypeID GetID() const {
        return id_;
    }

    [[nodiscard]]
    const char* GetName() const {
        return name_;
    }

    [[nodiscard]]
    const char* GetNamespace() const {
        if (namespace_.has_value()) {
            return namespace_.value();
        }
        return "";
    }

    [[nodiscard]]
    bool InNamespace() const {
        return namespace_.has_value();
    }

    [[nodiscard]]
    bool HasBases() const {
        return !bases_.empty();
    }
    
    [[nodiscard]]
    const std::vector<TypeID>& GetBases() const {
        return bases_;
    }

    [[nodiscard]]
    Result<const Type&> GetBase(const size_t index) const {
        return bases_[index].GetType();
    }
    
    [[nodiscard]]
    bool HasInners() const {
        return !inners_.empty();
    }

    [[nodiscard]]
    const std::vector<TypeID>& GetInners() const {
        return inners_;
    }

    [[nodiscard]]
    Result<const Type&> GetInner(const size_t index) const {
        return inners_[index].GetType();
    }

    [[nodiscard]]
    bool HasInner(const TypeID id) const {
        return std::ranges::any_of(inners_, [id](const TypeID t) {
            return t == id;
        });
    }

    [[nodiscard]]
    const TypeFlags& GetFlags() const {
        return flags_;
    }

    // fields

    [[nodiscard]]
    const std::vector<Field>& GetFields() const {
        return fields_;
    }

    std::optional<std::reference_wrapper<const Field>> GetField(const char* name) const {
        for (const auto& field : fields_) {
            if (field.GetName() == name) {
                return field;
            }
        }
        return std::nullopt;
    }

    // methods

    [[nodiscard]]
    const std::vector<Method>& GetMethods() const {
        return methods_;
    }

    std::optional<std::reference_wrapper<const Method>> GetMethod(const char* name) const {
        for (const auto& method : methods_) {
            if (method.GetName() == name) {
                return method;
            }
        }
        return std::nullopt;
    }

    // utils

    [[nodiscard]]
    bool Is(const TypeID id) const {
        return id_ == id;
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
        if (printFunc_ != nullptr) {
            return printFunc_(stream, *this);
        }

        if (InNamespace()) {
            stream << GetNamespace() << "::";
        }
        stream << GetName();
    }

    [[nodiscard]]
    std::string Dump() const {
        std::stringstream stream;
        if (printFunc_ != nullptr) {
            printFunc_(stream, *this);
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
