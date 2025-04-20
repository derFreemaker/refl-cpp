#pragma once

#include <vector>

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/type.hpp"
#include "refl-cpp/reflect_printer.hpp"

template <>
struct ReflCpp::ReflectData<void> {
    static TypeData Create() {
        return TypeData{ .name = "Void" };
    }
};

template <>
struct ReflCpp::ReflectPrinter<void> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "void";
    }
};

namespace ReflCpp {
struct ReflectionDatabase {
private:
    std::vector<std::unique_ptr<Type>> types_;

public:
    static const Type& Void() { // NOLINT(*-exception-escape)
        static std::optional<Type> type{};
        if (type.has_value()) {
            return type.value();
        }

        const TypeData data = ReflectData<void>::Create();
        constexpr auto id = TypeID(0);

        TypeOptions options{};
        options.printFunc = ReflectPrinter<void>::Print;

        type.emplace(id, data, options);

        return type.value();
    }

    static ReflectionDatabase& Instance() {
        static ReflectionDatabase instance;
        return instance;
    }

    template <typename T>
    TypeID RegisterType() {
        if (types_.size() == SIZE_MAX) {
            throw std::runtime_error(
                "Reflection database size is hitting more than 'SIZE_MAX'."
                "This is probably not normal and should be investigated."
            );
        }

        TypeData type_data = ReflectData<T>::Create();
        const auto type_id = TypeID(types_.size() + 1);

        TypeOptions type_options{};

        if constexpr (detail::HasReflectPrinter<T>) {
            type_options.printFunc = ReflectPrinter<T>::Print;
        }

        const auto& type = types_.emplace_back(std::make_unique<Type>(type_id, type_data, type_options));
        return type->GetID();
    }

    [[nodiscard]]
    const Type& GetType(const TypeID id) const {
        if (id.IsInvalid() || id > types_.size()) {
            throw std::logic_error("invalid type id");
        }

        return *types_[id - 1];
    }
};
}
