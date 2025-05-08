#pragma once

#include <cstdint>

#include "refl-cpp/declare_reflect.hpp"

namespace ReflCpp {

struct TypeID {
private:
    static constexpr auto invalid_ = 0;

    uint32_t id_ = invalid_;

public:
    static constexpr TypeID Invalid() noexcept {
        return { invalid_ };
    }

    [[nodiscard]]
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
    rescpp::result<const Type&, GetTypeError> GetType() const noexcept {
        if (IsInvalid()) {
            return rescpp::fail(GetTypeError::InvalidID);
        }

        return detail::Reflect(id_);
    }

    [[nodiscard]]
    bool Equals(const TypeID& other) const noexcept {
        return id_ == other.id_;
    }

    template <typename T>
    [[nodiscard]]
    bool Equals() const noexcept {
        const TypeID other = RESCPP_TRY_IMPL(ReflectID<T>(), {
            return false;
        });
        return Equals(other);
    }
};
}
