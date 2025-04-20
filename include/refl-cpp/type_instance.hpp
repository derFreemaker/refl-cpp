#pragma once

#include "refl-cpp/type.hpp"
#include "refl-cpp/database.hpp"

namespace ReflCpp {
template <typename T>
struct TypeInstance {
    static TypeID ID() {
        static TypeID id = TypeID::Invalid();
        if (id.IsValid()) {
            return id;
        }
        
        auto& database = ReflectionDatabase::Instance();
        id = database.RegisterType<T>();
        
        return id;
    }

    static const Type& Type() {
        const TypeID id = ID();
        const auto& database = ReflectionDatabase::Instance();
        return database.GetType(id);
    }
};

template <>
struct TypeInstance<void> {
    static TypeID ID() {
        return ReflectionDatabase::Void().GetID();
    }

    static const Type& Type() {
        return ReflectionDatabase::Void();
    }
};
}
