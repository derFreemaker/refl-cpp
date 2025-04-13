#pragma once

#include <vector>

#include "refl-cpp/common/result.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/type.hpp"
#include "refl-cpp/reflect_printer.hpp"

template <>
struct ReflCpp::ReflectData<void> {
    static TypeData Create() noexcept {
        return TypeData{ .name = "Void" };
    }
};

template <>
struct ReflCpp::ReflectPrinter<void> {
    static void Print(std::ostream& stream, const Type& type) noexcept {
        stream << "void";
    }
};

namespace ReflCpp {
struct ReflectionDatabase {
private:
    std::vector<std::unique_ptr<Type>> types_;

public:
    static const Type& Void() noexcept { // NOLINT(*-exception-escape)
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

    static ReflectionDatabase& Instance() noexcept {
        static ReflectionDatabase instance;
        return instance;
    }

    template <typename T_>
    Result<TypeID> RegisterType() noexcept {
        if (types_.size() == SIZE_MAX) {
            return {
                RESULT_ERROR(),
                "Reflection database size is hitting more than 'SIZE_MAX'."
                "This is probably not normal and should be investigated."
            };
        }

        TypeData type_data;
        // We wrap this, because inside the constructors we use for
        // creating the type data exceptions can be thrown.
        try {
            type_data = TRY(ReflectData<T_>::Create());
        }
        catch (const std::exception& e) {
            return { RESULT_ERROR(), "unable to create type data: {0}", e.what() };
        }
        const auto type_id = TypeID(types_.size() + 1);

        TypeOptions type_options{};

        if constexpr (detail::HasReflectPrinter<T_>) {
            type_options.printFunc = ReflectPrinter<T_>::Print;
        }

        try {
            const auto& type = types_.emplace_back(std::make_unique<Type>(type_id, type_data, type_options));
            return type->GetID();
        }
        catch (const std::exception& e) {
            return { RESULT_ERROR(), e.what() };
        }
    }

    [[nodiscard]]
    Result<const Type&> GetType(const TypeID id) const noexcept {
        if (id.IsInvalid() || id > types_.size()) {
            return { RESULT_ERROR(), "invalid type id" };
        }

        return *types_[id - 1];
    }
};
}
