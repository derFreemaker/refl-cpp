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

    [[nodiscard]]
    uint32_t Value() const {
        return id_;
    }

    operator uint32_t() const {
        return id_;
    }

    constexpr TypeID(const uint32_t id) noexcept
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
    const Type& GetType() const {
        if (IsInvalid()) {
            throw std::invalid_argument("invalid type id");
        }

        return detail::Reflect(id_);
    }

    operator const Type&() const {
        return GetType();
    }

    [[nodiscard]]
    bool Equals(const TypeID& other) const {
        return id_ == other.id_;
    }

    template <typename T>
    [[nodiscard]]
    bool Equals() const {
        const TypeID other = ReflectID<T>();
        return Equals(other);
    }
};
}
