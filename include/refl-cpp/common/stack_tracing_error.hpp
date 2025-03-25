#pragma once

#include "refl-cpp/common/formatted_error.hpp"

#ifdef NDEBUG
namespace ReflCpp {
using ResultError = FormattedError;
}
#else
#include <stacktrace>

namespace ReflCpp {
struct StackTracingError : FormattedError {
private:
    const std::stacktrace m_StackTrace;

public:
    template <typename... Args>
    StackTracingError(const std::stacktrace& stacktrace, const std::string_view& format, Args&&... args)
        : FormattedError(format, std::forward<Args>(args)...), m_StackTrace(stacktrace) {}

    StackTracingError(const std::stacktrace& stacktrace, const FormattedError& error)
        : FormattedError(error), m_StackTrace(stacktrace) {}

    [[nodiscard]]
    const std::stacktrace& StackTrace() const {
        return m_StackTrace;
    }

    void Str(std::ostream& stream) const {
        stream << m_Message << "\n" << m_StackTrace;
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
};

using ResultError = StackTracingError;
}
#endif
