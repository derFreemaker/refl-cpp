#pragma once

#include <vector>

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/type.hpp"
#include "refl-cpp/reflect_printer.hpp"

template <>
struct ReflCpp::ReflectPrinter<void> {
    static void Print(std::ostream& stream, const Type& _) {
        stream << "void";
    }
};

namespace ReflCpp {
namespace detail {
template <typename T>
concept HasReflectData = requires() {
    { ReflectData<T>::Create() } -> std::same_as<const TypeData&>;
};
}

struct ReflectionDatabase {
private:
    std::vector<std::unique_ptr<Type>> types_;

public:
    static const Type& Void() noexcept {
        static TypeData data{
            .name = "Void"
        };
        static Type type(TypeID(0), &data, {
                             .printFunc = ReflectPrinter<void>::Print
                         });
        return type;
    }

    static ReflectionDatabase& Instance() {
        static ReflectionDatabase instance;
        return instance;
    }

    template <typename T>
        requires detail::HasReflectData<T>
    rescpp::result<TypeID, ReflectError> RegisterType() noexcept {
        if (types_.size() == SIZE_MAX) {
            return rescpp::fail(ReflectError::MaxLimitReached);
        }

        const TypeData* type_data;
        try {
            type_data = &ReflectData<T>::Create();
            if (!type_data) {
                return rescpp::fail<ReflectError>(ReflectError::CreationFailed);
            }
        }
        catch (const std::exception&) {
            return rescpp::fail<ReflectError>(ReflectError::CreationFailed);
        }
        const auto type_id = TypeID(types_.size() + 1);

        TypeOptions type_options{};

        if constexpr (detail::HasReflectPrinter<T>) {
            type_options.printFunc = ReflectPrinter<T>::Print;
        }

        try {
            const auto& type = types_.emplace_back(std::make_unique<Type>(type_id, type_data, type_options));
            return type->GetID();
        }
        catch (const std::exception&) {
            return rescpp::fail<ReflectError>(ReflectError::OutOfMemory);
        }
    }

    [[nodiscard]]
    rescpp::result<const Type&, GetTypeError> GetType(const TypeID id) const noexcept {
        if (id.IsInvalid()) {
            return rescpp::fail(GetTypeError::InvalidID);
        }

        if (id.Value() > types_.size()) {
            return rescpp::fail(GetTypeError::NotFound);
        }

        return *types_[id - 1];
    }
};
}
