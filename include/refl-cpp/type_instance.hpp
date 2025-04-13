#pragma once

#include "refl-cpp/type.hpp"
#include "refl-cpp/database.hpp"

namespace ReflCpp {
template <typename T>
struct TypeInstance {
    static Result<TypeID> ID() noexcept {
        static TypeID id = TypeID::Invalid();
        if (id.IsValid()) {
            return id;
        }
        
        auto& database = ReflectionDatabase::Instance();
        id = TRY(database.RegisterType<T>());
        
        return id;
    }

    static Result<const Type&> Type() noexcept {
        const TypeID id = TRY(ID());
        const auto& database = ReflectionDatabase::Instance();
        return database.GetType(id);
    }
};

template <>
struct TypeInstance<void> {
    static Result<TypeID> ID() noexcept {
        return ReflectionDatabase::Void().GetID();
    }

    static Result<const Type&> Type() noexcept {
        return ReflectionDatabase::Void();
    }
};
}
