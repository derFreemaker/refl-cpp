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
    Type(Type&&) = delete;

    Type(const TypeID id, const TypeData& data, const TypeOptions& options) noexcept
        : id_(id),

          name_(data.name),
          namespace_(data._namespace),
          bases_(data.bases),
          inners_(data.inners),

          flags_(data.flags),

          methods_(data.methods),
          fields_(data.fields),

          printFunc_(options.printFunc) {}

    [[nodiscard]]
    TypeID GetID() const noexcept {
        return id_;
    }

    [[nodiscard]]
    const char* GetName() const noexcept {
        return name_;
    }

    [[nodiscard]]
    const char* GetNamespace() const noexcept { // NOLINT(*-exception-escape)
        if (namespace_.has_value()) {
            return namespace_.value();
        }
        return "";
    }

    [[nodiscard]]
    bool InNamespace() const noexcept {
        return namespace_.has_value();
    }

    [[nodiscard]]
    bool HasBases() const noexcept {
        return !bases_.empty();
    }

    [[nodiscard]]
    const std::vector<TypeID>& GetBases() const noexcept {
        return bases_;
    }

    [[nodiscard]]
    Result<const Type&> GetBase(const size_t index) const noexcept {
        return bases_[index].GetType();
    }

    [[nodiscard]]
    bool HasInners() const noexcept {
        return !inners_.empty();
    }

    [[nodiscard]]
    const std::vector<TypeID>& GetInners() const noexcept {
        return inners_;
    }

    [[nodiscard]]
    Result<const Type&> GetInner(const size_t index) const noexcept {
        return inners_[index].GetType();
    }

    [[nodiscard]]
    bool HasInner(const TypeID id) const noexcept {
        return std::ranges::any_of(inners_, [id](const TypeID t) {
            return t == id;
        });
    }

    [[nodiscard]]
    const TypeFlags& GetFlags() const noexcept {
        return flags_;
    }

    // fields

    [[nodiscard]]
    const std::vector<Field>& GetFields() const noexcept {
        return fields_;
    }

    Result<const Field&> GetField(const char* name) const noexcept {
        for (const auto& field : fields_) {
            if (field.GetName() == name) {
                return field;
            }
        }
        return { RESULT_ERROR(), "no field found with name: {0}", name };
    }

    // methods

    [[nodiscard]]
    const std::vector<Method>& GetMethods() const noexcept {
        return methods_;
    }

    Result<const Method&> GetMethod(const char* name) const noexcept {
        for (const auto& method : methods_) {
            if (method.GetName() == name) {
                return method;
            }
        }
        return { RESULT_ERROR(), "no function found with name: {0}", name };
    }

    // utils

    [[nodiscard]]
    bool Is(const TypeID id) const noexcept {
        return id_ == id;
    }

    [[nodiscard]]
    bool Is(const Type& type) const noexcept {
        return Is(type.GetID());
    }

    template <typename T>
    [[nodiscard]]
    bool Is() const noexcept {
        return Is(ReflectID<T>());
    }

    void Print(std::ostream& stream) const noexcept {
        if (printFunc_ != nullptr) {
            return printFunc_(stream, *this);
        }

        if (InNamespace()) {
            stream << GetNamespace() << "::";
        }
        stream << GetName();
    }

    [[nodiscard]]
    std::string Dump() const noexcept {
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

inline std::ostream& operator<<(std::ostream& stream, const ReflCpp::Type& type) noexcept {
    type.Print(stream);
    return stream;
}
