#pragma once

#include <optional>
#include <sstream>
#include <stacktrace>
#include <string>

#include <fmt/format.h>

#include "refl-cpp/common/type_traits.hpp"

namespace ReflCpp {
struct FormattedError {
protected:
    std::string m_Message;

public:
    explicit FormattedError(const std::string& message)
        : m_Message(message) {}

    template <typename... Args>
    FormattedError(const std::string_view& fmt_str, Args&&... args)
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
struct StackTracingError : FormattedError {
private:
    const std::stacktrace m_StackTrace;

public:
    template <typename... Args>
    StackTracingError(const std::stacktrace& stacktrace, const std::string_view& fmt_str, Args&&... args)
        : FormattedError(fmt_str, std::forward<Args>(args)...), m_StackTrace(stacktrace) {}

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
#endif

#ifdef _DEBUG
using ResultError = StackTracingError;
#else
using ResultError = FormattedError;
#endif

namespace detail {
struct ErrorTag {};

struct OkTag {};

inline constexpr ErrorTag Error{};
inline constexpr OkTag Ok{};

struct ResultBase {
private:
    const std::optional<ResultError> m_Error;

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
struct ValueResultBase : ResultBase {
protected:
    union {
        std::_Nontrivial_dummy_type p_Dummy;
        std::remove_cv_t<T_> p_Value;
    };

    void p_ThrowBadAccessException() const {
        throw std::runtime_error("Attempted to access value of an error Result. Error: " + this->Error().Str());
    }

public:
    explicit ValueResultBase(ErrorTag, const ResultError& error) noexcept
    // ReSharper disable once CppRedundantMemberInitializer
        : ResultBase(error), p_Dummy{} {}

    explicit ValueResultBase(OkTag, T_&& value) noexcept
        : p_Value(static_cast<T_>(std::forward<T_>(value))) {}

    //NOTE: for some reason we need this
    // and '... = default' won't do
    ~ValueResultBase() {}
};
}

template <typename T_>
struct Result;

template <>
struct Result<void> : detail::ResultBase {
    Result(detail::OkTag) {}

    template <typename... Args>
    Result(detail::ErrorTag,
#ifdef _DEBUG
           const std::stacktrace& stacktrace,
#endif
           const std::string_view& fmt_str, Args&&... args)
        : ResultBase(ResultError(
#ifdef _DEBUG
            stacktrace,
#endif
            fmt_str, std::forward<Args>(args)...)) {}

    Result(detail::ErrorTag,
#ifdef _DEBUG
           const std::stacktrace& stacktrace,
#endif
           const FormattedError& error)
        : ResultBase(ResultError(
#ifdef _DEBUG
            stacktrace,
#endif
            error)) {}

    Result(detail::ErrorTag, const ResultError& error)
        : ResultBase(error) {}
};

template <typename T_>
struct Result : detail::ValueResultBase<T_> {
    Result(detail::OkTag, T_& value) noexcept
        : detail::ValueResultBase<T_>(detail::Ok, std::forward<T_>(value)) {}

    Result(detail::OkTag, T_&& value) noexcept
        : detail::ValueResultBase<T_>(detail::Ok, std::forward<T_>(value)) {}

    template <typename... Args>
    Result(detail::ErrorTag,
#ifdef _DEBUG
           const std::stacktrace& stacktrace,
#endif
           const std::string_view& fmt_str, Args&&... args)
        : detail::ValueResultBase<T_>(detail::Error, ResultError(
#ifdef _DEBUG
                                          stacktrace,
#endif
                                          fmt_str, std::forward<Args>(args)...)) {}

    Result(detail::ErrorTag,
#ifdef _DEBUG
           const std::stacktrace& stacktrace,
#endif
           const FormattedError& error)
        : detail::ValueResultBase<T_>(ResultError(
#ifdef _DEBUG
            stacktrace,
#endif
            error)) {}

    Result(detail::ErrorTag, const ResultError& error)
        : detail::ValueResultBase<T_>(detail::Error, error) {}

    T_& Value() & {
        if (this->IsError()) {
            this->p_ThrowBadAccessException();
        }
        return this->p_Value;
    }

    make_const<T_>& Value() const & {
        if (this->IsError()) {
            this->p_ThrowBadAccessException();
        }
        return this->p_Value;
    }

    T_&& Value() && {
        if (this->IsError()) {
            this->p_ThrowBadAccessException();
        }
        return std::move(this->p_Value);
    }

    make_const<T_>&& Value() const && {
        if (this->IsError()) {
            this->p_ThrowBadAccessException();
        }
        return std::move(this->p_Value);
    }
};

template <typename T_>
struct Result<T_&> : detail::ValueResultBase<T_*> {
    Result(detail::OkTag, T_& value) noexcept
        : detail::ValueResultBase<T_*>(detail::Ok, &value) {}

    Result(detail::OkTag, T_* value) noexcept
        : detail::ValueResultBase<T_*>(detail::Ok, value) {}

    template <typename... Args>
    Result(detail::ErrorTag,
#ifdef _DEBUG
           const std::stacktrace& stacktrace,
#endif
           const std::string_view& fmt_str, Args&&... args)
        : detail::ValueResultBase<T_*>(detail::Error, ResultError(
#ifdef _DEBUG
                                           stacktrace,
#endif
                                           fmt_str, std::forward<Args>(args)...)) {}

    Result(detail::ErrorTag,
#ifdef _DEBUG
           const std::stacktrace& stacktrace,
#endif
           const FormattedError& error)
        : detail::ValueResultBase<T_*>(detail::Error, ResultError(
#ifdef _DEBUG
                                           stacktrace,
#endif
                                           error)) {}

    Result(detail::ErrorTag, const ResultError& error)
        : detail::ValueResultBase<T_*>(detail::Error, error) {}

    T_& Value() & {
        if (this->IsError()) {
            this->p_ThrowBadAccessException();
        }
        return *this->p_Value;
    }

    make_const<T_>& Value() const & {
        if (this->IsError()) {
            this->p_ThrowBadAccessException();
        }
        return *this->p_Value;
    }

    T_&& Value() && {
        if (this->IsError()) {
            this->p_ThrowBadAccessException();
        }
        return std::move(*this->p_Value);
    }

    make_const<T_>&& Value() const && {
        if (this->IsError()) {
            this->p_ThrowBadAccessException();
        }
        return std::move(*this->p_Value);
    }
};

//TODO: add custom result for references

namespace detail {
inline void TryHelper(const Result<void>* _) {}

template <typename T_>
T_& TryHelper(Result<T_>* result) {
    return result->Value();
}

template <typename T_>
make_const<T_>& TryHelper(const Result<T_>* result) {
    return result->Value();
}
}

//TODO: maybe include the expression as well for debug purposes
//TODO: improve debug information using std::stacktrace its unnecessary long
//TODO: create TRY macro for functions that don't return a result on error, maybe throw exception

// The TRY macro uses a little bit of a lifetime hack,
// since we use '_result' after its scope through a pointer.
// This only works because '_result' lives in the enclosing
// function stack.

#ifdef _DEBUG

#define RESULT_ERROR() \
    ::ReflCpp::detail::Error, std::stacktrace::current()

#define RESULT_OK() \
    ::ReflCpp::detail::Ok

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

#else

#define RESULT_ERROR() \
    ::ReflCpp::detail::Error

#define RESULT_OK() \
    ::ReflCpp::detail::Ok

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

#endif
}
