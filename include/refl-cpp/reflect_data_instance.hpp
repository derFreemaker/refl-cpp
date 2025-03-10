#pragma once

#include "refl-cpp/type.hpp"
#include "refl-cpp/database.hpp"

namespace ReflCpp {
template <typename T>
struct ReflectDataInstance {
    static const Type& Instance() {
        auto& database = ReflectionDatabase::Instance();
        static auto instance = database.RegisterType<T>().GetID();
        return database.GetType(instance);
    }
};
}
