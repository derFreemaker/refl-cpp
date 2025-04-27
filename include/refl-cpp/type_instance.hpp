#pragma once

#include "refl-cpp/type.hpp"
#include "refl-cpp/database.hpp"

namespace ReflCpp {
template <typename T>
struct TypeInstance {
    static rescpp::result<TypeID, ReflectError> ID() {
        static TypeID id = TypeID::Invalid();
        if (id.IsValid()) {
            return id;
        }
        
        auto& database = ReflectionDatabase::Instance();
        id = TRY(database.RegisterType<T>());
        
        return id;
    }

    static rescpp::result<const Type&, ReflectError> Type() {
        const TypeID& id = TRY(ID());
        const auto& database = ReflectionDatabase::Instance();
        return database.GetType(id).value();
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
