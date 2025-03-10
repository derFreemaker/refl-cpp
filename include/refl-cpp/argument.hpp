#pragma once

#include "refl-cpp/type_id.hpp"

namespace ReflCpp {
struct Type;

struct Argument {
    template <typename T>
    Argument(const char* name);

    [[nodiscard]]
    const char* GetName() const;
    
    [[nodiscard]]
    const Type& GetType() const;

private:
    const char* m_Name;
    const TypeID m_TypeID;
};
}
