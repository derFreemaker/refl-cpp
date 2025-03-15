#pragma once

#include "refl-cpp/type.hpp"
#include "refl-cpp/database.hpp"

namespace ReflCpp {
template <typename T>
struct TypeInstance {
    static TypeID ID() {
        auto& database = ReflectionDatabase::Instance();
        static auto id = database.RegisterType<T>();
        return id;
    }

    static const Type& Type() {
        const auto& database = ReflectionDatabase::Instance();
        return database.GetType(ID());
    }
};
}
