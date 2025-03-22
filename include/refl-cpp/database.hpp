#pragma once

#include <vector>

#include "refl-cpp/common/result.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/type.hpp"
#include "refl-cpp/reflect_printer.hpp"

namespace ReflCpp {
struct ReflectionDatabase {
private:
    std::vector<std::unique_ptr<Type>> m_types_data;

public:
    static ReflectionDatabase& Instance() {
        static ReflectionDatabase instance;
        return instance;
    }

    template <typename T_>
    Result<TypeID> RegisterType() {
        if (m_types_data.size() == SIZE_MAX) {
            return {
                RESULT_ERROR(),
                "Reflection database size is hitting more than 'SIZE_MAX'."
                "This is probably not normal and should be investigated."
            };
        }

        TypeData type_data = TRY(ReflectData<T_>::Create());
        const auto type_id = TypeID(m_types_data.size() + 1);

        TypeOptions type_options {};

        if constexpr (detail::HasReflectPrinter<T_>) {
            type_options.print_func = ReflectPrinter<T_>::Print;
        }

        const auto& type = m_types_data.emplace_back(std::make_unique<Type>(type_id, type_data, type_options));
        return { RESULT_OK(), type->GetID() };
    }

    [[nodiscard]]
    Result<const Type&> GetType(const TypeID id) const {
        if (id.IsInvalid() || id > m_types_data.size()) {
            return { RESULT_ERROR(), "invalid type id" };
        }

        return { RESULT_OK(), *m_types_data[id - 1] };
    }
};
}
