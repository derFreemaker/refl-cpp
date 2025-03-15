#pragma once

#include <stdexcept>
#include <vector>

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
    TypeID RegisterType() {
        const TypeData& type_data = ReflectData<T_>::Create();
        const auto type_id = TypeID(m_types_data.size() + 1);

        TypeOptions type_options{};
        
        if constexpr (detail::HasReflectPrinter<T_>) {
            type_options.print_func = ReflectPrinter<T_>::Print;
        }
        
        const auto type = new Type(type_id, type_data, type_options);
        return (*m_types_data.emplace_back(type)).GetID();
    }

    [[nodiscard]]
    const Type& GetType(const TypeID id) const {
        if (id.IsInvalid() || id > m_types_data.size()) {
            throw std::invalid_argument("Invalid type");
        }

        return *m_types_data[id - 1];
    }
};
}
