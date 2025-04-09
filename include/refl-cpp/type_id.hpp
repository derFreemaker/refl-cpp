#pragma once

#include <cstdint>

#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct TypeID {
private:
    static constexpr auto invalid_ = 0;

    uint32_t id_ = invalid_;

public:
    static constexpr auto Invalid() {
        return TypeID(invalid_);
    }

    operator uint32_t() const {
        return id_;
    }
    
    constexpr TypeID(const uint32_t id)
        : id_(id) {}

    bool operator==(const TypeID other) const {
        return id_ == other.id_;
    }

    [[nodiscard]]
    bool IsValid() const {
        return this->id_ != invalid_;
    }

    [[nodiscard]]
    bool IsInvalid() const {
        return this->id_ == invalid_;
    }

    [[nodiscard]]
    Result<const Type&> GetType() const {
        if (IsInvalid()) {
            return { RESULT_ERROR(), "invalid type id" };
        }

        return detail::Reflect(id_);
    }
    
    operator Result<const Type&>() const {
        return GetType();
    }

    template <typename T_>
    [[nodiscard]]
    bool Equals() const {
        const auto result = ReflectID<T_>();
        const auto other = result.Value();
        return !result.HasError() && id_ == other.id_;
    }
};
}
