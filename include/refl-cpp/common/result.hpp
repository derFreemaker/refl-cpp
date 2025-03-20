#pragma once

#include <string>
#include <optional>

#include "fmt/base.h"
#include "fmt/format.h"

namespace ReflCpp {
struct FormattedError {
private:
    std::string message;

public:
    explicit FormattedError(std::string msg) : message(std::move(msg)) {}

    template <typename... Args>
    FormattedError(const std::string_view fmt_str, Args&&... args)
        : message(fmt::format(fmt::runtime(fmt_str), std::forward<Args>(args)...)) {}

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

inline constexpr detail::ErrorTag Error{};
inline constexpr detail::OkTag Ok{};

template <typename T_>
struct Result;

namespace detail {
struct ResultBase {
private:
    const std::optional<FormattedError> m_Error;

public:
    ResultBase() noexcept = default;

    ResultBase(const FormattedError& error) noexcept
        : m_Error(error) {}

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
            throw std::runtime_error("Attempted to access error of a success Result");
        }
        return m_Error.value();
    }
};

template <typename T_>
struct ValueResultBase : public ResultBase {
protected:
    union {
        std::_Nontrivial_dummy_type m_Dummy;
        std::remove_cv_t<T_> m_Value;
    };

public:
    explicit ValueResultBase(ErrorTag, const FormattedError& error) noexcept
        : ResultBase(error), m_Dummy{} {}

    template <typename T2_ = T_>
    explicit ValueResultBase(OkTag, T2_&& value) noexcept // NOLINT(*-forwarding-reference-overload)
        : m_Value(std::forward<T2_>(value)) {}

    ~ValueResultBase() {}
};
}

template <>
struct Result<void> : detail::ResultBase {
    Result(detail::OkTag) {}

    template <typename... Args>
    Result(detail::ErrorTag, const std::string_view fmt_str, Args&&... args)
        : ResultBase(FormattedError(fmt_str, std::forward<Args>(args)...)) {}

    template <typename OtherT_>
    Result(detail::ErrorTag, const Result<OtherT_>& result)
        : ResultBase(result.Error()) {}

    // dummy functions for TRY macro
    static constexpr void Value() {}
    void operator*() const & {
        return Value();
    }
};

template <typename T_>
struct Result : detail::ValueResultBase<T_> {
    template <typename T2_ = T_>
        requires std::is_convertible_v<T2_, T_>
        && std::is_nothrow_constructible_v<T_, T2_> //TODO: add more limitation
    Result(detail::OkTag, T2_&& value) noexcept
        : detail::ValueResultBase<T_>(Ok, value) {}

    template <typename... Args>
    Result(detail::ErrorTag, const std::string_view fmt_str, Args&&... args)
        : detail::ValueResultBase<T_>(Error, FormattedError(fmt_str, args...)) {}

    template <typename OtherT_>
    Result(detail::ErrorTag, const Result<OtherT_>& result)
        : detail::ValueResultBase<T_>(Error, result.Error()) {}
        
    const T_& Value() const & {
        if (this->IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return this->m_Value;
    }

    const T_& operator*() const & {
        return Value();
    }
};

template <typename T_>
struct Result<T_&> : detail::ValueResultBase<T_*> {
    Result(detail::OkTag, T_& value) noexcept
        : detail::ValueResultBase<T_*>(Ok, &value) {}

    template <typename... Args>
    Result(detail::ErrorTag, const std::string_view fmt_str, Args&&... args)
        : detail::ValueResultBase<T_*>(Error, FormattedError(fmt_str, args...)) {}

    template <typename OtherT_>
    Result(detail::ErrorTag, const Result<OtherT_>& result)
        : detail::ValueResultBase<T_*>(Error, result.Error()) {}

    const T_& Value() const & {
        if (this->IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return *this->m_Value;
    }

    operator const T_&() const & {
        return Value();
    }

    const T_& operator*() const & {
        return Value();
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
