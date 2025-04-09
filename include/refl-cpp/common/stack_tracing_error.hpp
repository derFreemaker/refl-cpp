#pragma once

#include "refl-cpp/common/formatted_error.hpp"

#if REFLCPP_ENABLE_STACK_TRACING_ERROR == 0
namespace ReflCpp {
using ResultError = FormattedError;
}
#else
#include <stacktrace>

namespace ReflCpp {
struct StackTracingError {
private:
    const FormattedError formatted_;
    const std::stacktrace stacktrace_;

public:
    template <typename... Args>
    StackTracingError(std::stacktrace stacktrace, const std::string_view& format, Args&&... args) noexcept
        : formatted_(format, std::forward<Args>(args)...), stacktrace_(std::move(stacktrace)) {}

    StackTracingError(std::stacktrace stacktrace, FormattedError error) noexcept
        : formatted_(std::move(error)), stacktrace_(std::move(stacktrace)) {}

    StackTracingError(const StackTracingError& other) noexcept = default;

    StackTracingError& operator=(const StackTracingError& other) noexcept {
        if (this != &other) {
            this->~StackTracingError();
            new(this) StackTracingError(other);
        }
        return *this;
    }

    [[nodiscard]]
    const std::string& Message() const noexcept {
        return formatted_.Message();
    }

    [[nodiscard]]
    const std::stacktrace& StackTrace() const noexcept {
        return stacktrace_;
    }

    void Str(std::ostream& stream) const noexcept {
        try {
            stream << formatted_.Message() << "\n" << stacktrace_;
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

    friend std::ostream& operator<<(std::ostream& os, const StackTracingError& error) noexcept {
        error.Str(os);
        return os;
    }

    template <typename T_>
    operator Result<T_>() const noexcept {
        return Result<T_>(detail::Error, *this);
    }
};

using ResultError = StackTracingError;
}
#endif
