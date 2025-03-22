#pragma once

#include "refl-cpp/type.hpp"
#include "refl-cpp/database.hpp"

namespace ReflCpp {
template <typename T>
struct TypeInstance {
    static Result<TypeID> ID() {
        static TypeID id = TypeID::Invalid();
        if (id.IsValid()) {
            return { RESULT_OK(), id };
        }
        
        auto& database = ReflectionDatabase::Instance();
        id = TRY(database.RegisterType<T>());
        
        return { RESULT_OK(), id };
    }

    static Result<const Type&> Type() {
        const TypeID id = TRY(ID());
        const auto& database = ReflectionDatabase::Instance();
        return database.GetType(id);
    }
};
}
