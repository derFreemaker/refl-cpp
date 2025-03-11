#pragma once

#include "refl-cpp/type.hpp"
#include "refl-cpp/database.hpp"

namespace ReflCpp {
template <typename T>
struct ReflectDataInstance {
    static TypeID ID() {
        auto& database = ReflectionDatabase::Instance();
        static auto instance = database.RegisterType<T>().GetID();
        return instance;
    }

    static const Type& Instance() {
        const auto& database = ReflectionDatabase::Instance();
        return database.GetType(ID());
    }
};
}
