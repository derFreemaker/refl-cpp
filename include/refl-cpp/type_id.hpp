#pragma once

#include <cstdint>

#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {
struct TypeID {
private:
    static constexpr auto invalid_ = 0;

    uint32_t id_ = invalid_;

public:
    static constexpr auto Invalid() noexcept {
        return TypeID(invalid_);
    }

    uint32_t Value() const noexcept {
        return id_;
    }
    
    operator uint32_t() const noexcept {
        return id_;
    }

    constexpr TypeID(const uint32_t id) noexcept
        : id_(id) {}

    bool operator==(const TypeID other) const noexcept {
        return id_ == other.id_;
    }

    [[nodiscard]]
    bool IsValid() const noexcept {
        return this->id_ != invalid_;
    }

    [[nodiscard]]
    bool IsInvalid() const noexcept {
        return this->id_ == invalid_;
    }

    [[nodiscard]]
    Result<const Type&> GetType() const noexcept {
        if (IsInvalid()) {
            return { RESULT_ERROR(), "invalid type id" };
        }

        return detail::Reflect(id_);
    }

    operator Result<const Type&>() const noexcept {
        return GetType();
    }

    template <typename T_>
    [[nodiscard]]
    bool Equals() const noexcept {
        auto result = ReflectID<T_>();
        const TypeID other = TRY_IMPL(
            result,
            return false;
        );
        return id_ == other.id_;
    }
};
}
