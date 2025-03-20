#pragma once

#include <string>
#include <optional>
#include <variant>

#include "fmt/base.h"
#include "fmt/format.h"

namespace ReflCpp {
struct FormattedError {
private:
    std::string message;

public:
    explicit FormattedError(std::string msg) : message(std::move(msg)) {}

    template <typename... Args>
    FormattedError(fmt::format_string<Args...> fmt_str, Args&&... args)
        : message(fmt::format(fmt_str, std::forward<Args>(args)...)) {}

    [[nodiscard]]
    const std::string& Str() const {
        return message;
    }

    operator std::string() const {
        return message;
    }

    friend std::ostream& operator<<(std::ostream& os, const FormattedError& error) {
        return os << error.message;
    }
};

namespace detail {
struct ErrorTag {};

struct OkTag {};
}

inline constexpr detail::ErrorTag Error {};
inline constexpr detail::OkTag Ok {};

template <typename T_>
struct Result {
private:
    const bool m_IsSuccess;

    std::variant<T_, const FormattedError> m_Data;

public:
    Result(detail::OkTag, const T_& result)
        : m_IsSuccess(true),
          m_Data(result) {}

    template <typename... Args>
    Result(detail::ErrorTag, fmt::format_string<Args...> fmt_str, Args&&... args)
        : m_IsSuccess(false),
          m_Data(FormattedError(fmt_str, args...)) {}

    template <typename OtherT_>
    Result(detail::ErrorTag, const Result<OtherT_>& result)
        : m_IsSuccess(false),
          m_Data(result.m_Data) {}

    [[nodiscard]]
    bool IsSuccess() const {
        return m_IsSuccess;
    }

    [[nodiscard]]
    bool IsError() const {
        return !m_IsSuccess;
    }

    T_& Value() & {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return std::get<T_>(m_Data);
    }

    const T_& Value() const & {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return std::get<T_>(m_Data);
    }
    
    T_&& Value() && {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return std::move(std::get<T_>(m_Data));
    }

    const T_&& Value() const && {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return std::move(std::get<T_>(m_Data));
    }
    
    [[nodiscard]]
    const FormattedError& Error() const & {
        if (IsSuccess()) {
            throw std::runtime_error("Attempted to access error of a success Result");
        }
        return std::get<const FormattedError>(m_Data);
    }
};

template <>
struct Result<void> {
private:
    const std::optional<FormattedError> m_Error;

public:
    Result(detail::OkTag) {}

    template <typename... Args>
    Result(detail::ErrorTag, fmt::format_string<Args...> fmt_str, Args&&... args)
        : m_Error(FormattedError(fmt_str, std::forward<Args>(args)...)) {}

    template <typename OtherT_>
    Result(detail::ErrorTag, const Result<OtherT_>& result)
        : m_Error(result.Error()) {}

    [[nodiscard]]
    bool IsSuccess() const {
        return !m_Error.has_value();
    }

    [[nodiscard]]
    bool IsError() const {
        return m_Error.has_value();
    }

    [[nodiscard]]
    const FormattedError& Error() const & {
        if (IsSuccess()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return *m_Error;
    }
};

template <typename T_>
struct Result<T_&> {
private:
    const bool m_IsSuccess;
    std::variant<std::reference_wrapper<T_>, const FormattedError> m_Data;

public:
    // Constructor for success
    Result(detail::OkTag, T_& result)
        : m_IsSuccess(true),
          m_Data(result) {}

    // Constructor for error
    template <typename... Args>
    Result(detail::ErrorTag, fmt::format_string<Args...> fmt_str, Args&&... args)
        : m_IsSuccess(false),
          m_Data(FormattedError(fmt_str, args...)) {}

    template <typename OtherT_>
    Result(detail::ErrorTag, const Result<OtherT_>& result)
        : m_IsSuccess(false),
          m_Data(result.Error()) {}

    [[nodiscard]]
    bool IsSuccess() const {
        return m_IsSuccess;
    }

    [[nodiscard]]
    bool IsError() const {
        return !m_IsSuccess;
    }

    std::reference_wrapper<T_>& Value() & {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return std::get<std::reference_wrapper<T_>>(m_Data);
    }
    
    const std::reference_wrapper<T_>& Value() const & {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return std::get<std::reference_wrapper<T_>>(m_Data);
    }

    std::reference_wrapper<T_>&& Value() && {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return std::move(std::get<std::reference_wrapper<T_>>(m_Data));
    }

    const std::reference_wrapper<T_>&& Value() const && {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return std::move(std::get<std::reference_wrapper<T_>>(m_Data));
    }

    [[nodiscard]]
    const FormattedError& Error() const & {
        if (IsSuccess()) {
            throw std::runtime_error("Attempted to access error of a success Result");
        }
        return std::get<const FormattedError>(m_Data);
    }
};

#define TRY(expr) ({ \
    auto _result = (expr); \
    if (_result.IsError()) { \
        return { ::ReflCpp::Error, _result }; \
    } \
    _result.Value(); \
})
}
