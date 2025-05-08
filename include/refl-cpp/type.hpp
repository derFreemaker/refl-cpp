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
    const TypeData* data_;
    const ReflectPrintFunc printFunc_;

public:
    Type() = delete;
    Type(const Type&) = delete;
    Type(Type&&) = delete;

    Type(const TypeID id, const TypeData* data, const TypeOptions& options) noexcept
        : id_(id),
          data_(data),
          printFunc_(options.printFunc) {}

    [[nodiscard]]
    TypeID GetID() const noexcept {
        return id_;
    }

    [[nodiscard]]
    const char* GetName() const noexcept {
        return data_->name;
    }

    [[nodiscard]]
    const char* GetNamespace() const noexcept { // NOLINT(*-exception-escape)
        if (data_->_namespace.has_value()) {
            return data_->_namespace.value();
        }
        return "::";
    }

    [[nodiscard]]
    bool InNamespace() const noexcept {
        return data_->_namespace.has_value();
    }

    [[nodiscard]]
    bool HasBases() const noexcept {
        return !data_->bases.empty();
    }

    [[nodiscard]]
    const std::vector<TypeID>& GetBases() const noexcept {
        return data_->bases;
    }

    [[nodiscard]]
    rescpp::result<const Type&, GetTypeError> GetBase(const size_t index) const {
        return data_->bases[index].GetType();
    }

    [[nodiscard]]
    bool HasInners() const noexcept {
        return !data_->inners.empty();
    }

    [[nodiscard]]
    const std::vector<TypeID>& GetInners() const noexcept {
        return data_->inners;
    }

    [[nodiscard]]
    rescpp::result<const Type&, GetTypeError> GetInner(const size_t index) const noexcept {
        return data_->inners[index].GetType();
    }

    [[nodiscard]]
    bool HasInner(const TypeID id) const noexcept {
        for (const auto& inner : data_->inners) {
            if (inner == id) {
                return true;
            }
        }
        return false;
    }

    [[nodiscard]]
    const TypeFlags& GetFlags() const noexcept {
        return data_->flags;
    }

    // fields

    [[nodiscard]]
    const std::vector<Field>& GetFields() const noexcept {
        return data_->fields;
    }

    std::optional<std::reference_wrapper<const Field>> GetField(const char* name) const noexcept {
        for (const auto& field : data_->fields) {
            if (field.GetName() == name) {
                return field;
            }
        }
        return std::nullopt;
    }

    // methods

    [[nodiscard]]
    const std::vector<Method>& GetMethods() const noexcept {
        return data_->methods;
    }

    std::optional<std::reference_wrapper<const Method>> GetMethod(const char* name) const noexcept {
        for (const auto& method : data_->methods) {
            if (method.GetName() == name) {
                return method;
            }
        }
        return std::nullopt;
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

    void Print(std::ostream& stream) const {
        if (printFunc_ != nullptr) {
            printFunc_(stream, *this);
            return;
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
