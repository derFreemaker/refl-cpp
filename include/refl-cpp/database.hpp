#pragma once

#include <stdexcept>
#include <vector>

#include "refl-cpp/type_id.hpp"
#include "refl-cpp/type.hpp"

namespace ReflCpp {
struct ReflectionDatabase {
private:
    std::vector<Type*> m_types_data;

public:
    static ReflectionDatabase& Instance() {
        static ReflectionDatabase instance;
        return instance;
    }

    ~ReflectionDatabase() {
        for (const auto type : m_types_data) {
            delete type;
        }
    }

    template <typename T>
    const Type& RegisterType() {
        const TypeData& type_data = ReflectData<T>::Create();
        const auto type_id = TypeID(m_types_data.size() + 1);

        TypeOptions type_options{};
        
        if constexpr (detail::HasReflectPrinter<T>) {
            type_options.print_func = ReflectPrinter<T>::Print;
        }

        const auto type = new Type(type_id, type_data, type_options);
        m_types_data.push_back(type);
        return *type;
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
