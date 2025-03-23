#pragma once

#include <string>
#include <optional>
#include <sstream>
#include <stacktrace>

#include "refl-cpp/common/type_traits.hpp"

#include "fmt/base.h"
#include "fmt/format.h"

namespace ReflCpp {
struct FormattedError {
protected:
    std::string m_Message;

public:
    explicit FormattedError(const std::string& msg) : m_Message(msg) {}

    template <typename... Args>
    FormattedError(const std::string_view fmt_str, Args&&... args)
        : m_Message(fmt::format(fmt::runtime(fmt_str), std::forward<Args>(args)...)) {}

    [[nodiscard]]
    const std::string& Message() const {
        return m_Message;
    }

    void Str(std::ostream& stream) const {
        stream << m_Message;
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

    friend std::ostream& operator<<(std::ostream& os, const FormattedError& error) {
        error.Str(os);
        return os;
    }
};

#ifdef _DEBUG
namespace detail {
struct StackTraceErrorTag {};

inline constexpr StackTraceErrorTag StackTraceError{};
}

struct StackTracEntry {
    const std::string file;
    const int line;
};

struct StackTracingError : FormattedError {
    using FormattedError::FormattedError;

private:
    const std::stacktrace m_StackTrace;

public:
    template <typename... Args>
    StackTracingError(detail::StackTraceErrorTag, const std::stacktrace& stacktrace, const std::string_view fmt_str, Args&&... args)
        : FormattedError(fmt_str, std::forward<Args>(args)...), m_StackTrace(stacktrace) {}
    
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
#else
using ResultError = FormattedError;
#endif

namespace detail {
struct ErrorTag {};

struct OkTag {};

inline constexpr ErrorTag Error{};
inline constexpr OkTag Ok{};
}

template <typename T_>
struct Result;

namespace detail {
struct ResultBase {
private:
    const std::optional<ResultError> m_Error;

protected:
    void p_ThrowBadAccessException() const {
        throw std::runtime_error("Attempted to access value of an error Result. Error: " + this->Error().Str());
    }

public:
    ResultBase(const ResultBase&) = delete;
    ResultBase(const ResultBase&&) = delete;

    ResultBase() noexcept = default;

    ResultBase(const ResultError& error) noexcept
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
    const ResultError& Error() const & {
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
    explicit ValueResultBase(ErrorTag, const ResultError& error) noexcept
        : ResultBase(error), m_Dummy{} {}

    template <typename T2_ = T_>
    explicit ValueResultBase(OkTag, T2_& value) noexcept // NOLINT(*-forwarding-reference-overload)
        : m_Value(std::forward<T2_>(value)) {}
    
    template <typename T2_ = T_>
    explicit ValueResultBase(OkTag, T2_&& value) noexcept // NOLINT(*-forwarding-reference-overload)
        : m_Value(std::forward<T2_>(value)) {}

    ~ValueResultBase() {}
};
}

template <>
struct Result<void> : detail::ResultBase {
    Result(detail::OkTag) {}

#ifdef _DEBUG
    template <typename... Args>
    Result(detail::ErrorTag, std::stacktrace stacktrace, const std::string_view fmt_str, Args&&... args)
        : ResultBase(ResultError(detail::StackTraceError, stacktrace, fmt_str, std::forward<Args>(args)...)) {}

    Result(detail::ErrorTag, const ResultError& error)
        : ResultBase(error) {}
#else
    template <typename... Args>
    Result(detail::ErrorTag, const std::string_view fmt_str, Args&&... args)
        : ResultBase(ResultError(fmt_str, std::forward<Args>(args)...)) {}

    template <typename OtherT_>
    Result(detail::ErrorTag, const Result<OtherT_>& result)
        : ResultBase(result.Error()) {}
#endif
};

template <typename T_>
struct Result : detail::ValueResultBase<T_> {
    template <typename T2_ = T_>
        requires std::is_convertible_v<T2_, T_>
        && std::is_nothrow_constructible_v<T_, T2_> //TODO: add more limitation
    Result(detail::OkTag, T2_&& value) noexcept
        : detail::ValueResultBase<T_>(detail::Ok, value) {}

#ifdef _DEBUG
    template <typename... Args>
    Result(detail::ErrorTag, const std::stacktrace& stacktrace, const std::string_view fmt_str, Args&&... args)
        : detail::ValueResultBase<T_>(detail::Error, ResultError(detail::StackTraceError, stacktrace, fmt_str, args...)) {}

    Result(detail::ErrorTag, const ResultError& error)
        : detail::ValueResultBase<T_>(detail::Error, error) {}
#else
    template <typename... Args>
    Result(detail::ErrorTag, const std::string_view fmt_str, Args&&... args)
        : detail::ValueResultBase<T_>(detail::Error, ResultError(fmt_str, args...)) {}

    template <typename OtherT_>
    Result(detail::ErrorTag, const Result<OtherT_>& result)
        : detail::ValueResultBase<T_>(detail::Error, result.Error()) {}
#endif

    T_& Value() & {
        if (this->IsError()) {
            p_ThrowBadAccessException();
        }
        return this->m_Value;
    }

    make_const_t<T_>& Value() const & {
        if (this->IsError()) {
            p_ThrowBadAccessException();
        }
        return this->m_Value;
    }

    T_& Value() && {
        if (this->IsError()) {
            p_ThrowBadAccessException();
        }
        return this->m_Value;
    }

    make_const_t<T_>& Value() const && {
        if (this->IsError()) {
            p_ThrowBadAccessException();
        }
        return this->m_Value;
    }
};

template <typename T_>
struct Result<T_&> : detail::ValueResultBase<T_*> {
    Result(detail::OkTag, T_& value) noexcept
        : detail::ValueResultBase<T_*>(detail::Ok, &value) {}

#ifdef _DEBUG
    template <typename... Args>
    Result(detail::ErrorTag, const std::stacktrace& stacktrace, const std::string_view fmt_str, Args&&... args)
        : detail::ValueResultBase<T_*>(detail::Error, ResultError(detail::StackTraceError, stacktrace, fmt_str, args...)) {}

    Result(detail::ErrorTag, const ResultError& error)
        : detail::ValueResultBase<T_*>(detail::Error, error) {}
#else
    template <typename... Args>
    Result(detail::ErrorTag, const std::string_view fmt_str, Args&&... args)
        : detail::ValueResultBase<T_*>(detail::Error, ResultError(fmt_str, args...)) {}
    
    template <typename OtherT_>
    Result(detail::ErrorTag, const Result<OtherT_>& result)
        : detail::ValueResultBase<T_*>(detail::Error, result.Error()) {}
#endif

    T_& Value() & {
        if (this->IsError()) {
            p_ThrowBadAccessException();
        }
        return *this->m_Value;
    }

    make_const_t<T_>& Value() const & {
        if (this->IsError()) {
            p_ThrowBadAccessException();
        }
        return *this->m_Value;
    }

    T_& Value() && {
        if (this->IsError()) {
            p_ThrowBadAccessException();
        }
        return *this->m_Value;
    }

    make_const_t<T_>& Value() const && {
        if (this->IsError()) {
            p_ThrowBadAccessException();
        }
        return *this->m_Value;
    }
};

namespace detail {
inline void TryHelper(const Result<void>* _) {}

template <typename T_>
T_& TryHelper(Result<T_>* result) {
    return result->Value();
}

template <typename T_>
T_& TryHelper(Result<T_&>* result) {
    return result->Value();
}

template <typename T_>
make_const<T_>& TryHelper(const Result<T_>* result) {
    return result->Value();
}

template <typename T_>
make_const_t<T_>& TryHelper(const Result<T_&>* result) {
    return result->Value();
}
}

//TODO: maybe include the expression as well for debug purposes
//TODO: improve debug information without using std::stacktrace since its unnecessary long
//TODO: create TRY macro for functions that don't return a result, maybe throw exception

// The TRY macro uses a little bit of a lifetime hack,
// since we use '_result' after its scope through a pointer.
// This only works because '_result' lives in the enclosing
// function stack.

#ifdef _DEBUG
#define TRY(...) \
    (::ReflCpp::detail::TryHelper( \
        ({ \
            auto _result = (__VA_ARGS__); \
            if (_result.IsError()) { \
                return { ::ReflCpp::detail::Error, _result.Error() }; \
            } \
            &_result; \
        }) \
    ))

#define RESULT_ERROR() \
    ::ReflCpp::detail::Error, std::stacktrace::current()

#define RESULT_OK() \
    ::ReflCpp::detail::Ok

#else
#define TRY(...) \
    (::ReflCpp::detail::TryHelper( \
        ({ \
            auto _result = (__VA_ARGS__); \
            if (_result.IsError()) { \
                return { ::ReflCpp::detail::Error, _result }; \
            } \
            &_result; \
        }) \
    ))

#define RESULT_ERROR() \
    ::ReflCpp::detail::Error

#define RESULT_OK() \
    ::ReflCpp::detail::Ok

#endif
}
