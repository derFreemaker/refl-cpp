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
    StackTracingError(std::stacktrace stacktrace, const std::string_view& format, Args&&... args)
        : formatted_(format, std::forward<Args>(args)...), stacktrace_(std::move(stacktrace)) {}

    StackTracingError(std::stacktrace stacktrace, FormattedError error)
        : formatted_(std::move(error)), stacktrace_(std::move(stacktrace)) {}

    StackTracingError(const StackTracingError& other) = default;

    StackTracingError& operator=(const StackTracingError& other) noexcept {
        if (this != &other) {
            this->~StackTracingError();
            new(this) StackTracingError(other);
        }
        return *this;
    }

    [[nodiscard]]
    const std::string& Message() const {
        return formatted_.Message();
    }

    [[nodiscard]]
    const std::stacktrace& StackTrace() const {
        return stacktrace_;
    }

    void Str(std::ostream& stream) const {
        stream << formatted_.Message() << "\n" << stacktrace_;
    }

    [[nodiscard]]
    std::string Str() const {
        std::stringstream stream;
        this->Str(stream);
        return stream.str();
    }

    operator std::string() const {
        return Str();
    }

    friend std::ostream& operator<<(std::ostream& os, const StackTracingError& error) {
        error.Str(os);
        return os;
    }

    template <typename T_>
    operator Result<T_>() const {
        return Result<T_>(detail::Error, *this);
    }
};

using ResultError = StackTracingError;
}
#endif
