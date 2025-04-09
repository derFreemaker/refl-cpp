#pragma once

#include <print>
#include <sstream>
#include <string>
#include <utility>

#include <fmt/format.h>

#include "refl-cpp/common/result_tags.hpp"

namespace ReflCpp {
template <typename T_>
struct Result;

struct FormattedError {
private:
    std::string msg_;

public:
    explicit FormattedError(std::string msg) noexcept
        : msg_(std::move(msg)) {}

    template <typename... Args>
    FormattedError(const std::string_view& format, Args&&... args) noexcept {
        try {
            msg_ = fmt::format(fmt::runtime(format), std::forward<Args>(args)...);
        }
        catch (const std::exception& e) {
            msg_ = std::string("exception: ") + e.what();
        }
    }

    [[nodiscard]]
    const std::string& Message() const noexcept {
        return msg_;
    }

    void Str(std::ostream& stream) const noexcept {
        try {
            stream << Message();
        }
        catch (const std::exception& _) {
            // we silently don't do anything
#ifndef NDEBUG
            __debugbreak();
#endif
        }
    }

    [[nodiscard]]
    std::string Str() const noexcept {
        try {
            std::stringstream stream;
            this->Str(stream);
            return stream.str();
        }
        catch (const std::exception& _) {
            // we silently don't do anything
#ifndef NDEBUG
            __debugbreak();
#endif
        }
        
        return "";
    }

    operator std::string() const noexcept {
        return Str();
    }

    friend std::ostream& operator<<(std::ostream& os, const FormattedError& error) noexcept {
        error.Str(os);
        return os;
    }

    template <typename T_>
    operator Result<T_>() const noexcept {
        return Result<T_>(detail::Error, *this);
    }
};
}
