#pragma once

#include "refl-cpp/type_id.hpp"

namespace ReflCpp {
struct Type;

struct ArgumentInfo {
    const char* name;
    const TypeID type;
};

struct Argument {
private:
    void* m_Data;
    const TypeID m_Type;

public:
    template <typename T>
    Argument(T& data);

    [[nodiscard]]
    const Type& GetType() const;

    [[nodiscard]]
    void* GetData() const {
        return m_Data;
    }
};
}
