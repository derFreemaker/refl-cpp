#pragma once

#include <vector>

#include "refl-cpp/common/result.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/type.hpp"
#include "refl-cpp/reflect_printer.hpp"

namespace ReflCpp {
struct ReflectionDatabase {
private:
    std::vector<std::unique_ptr<Type>> types_;

public:
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

        TypeData type_data = TRY(ReflectData<T_>::Create());
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
