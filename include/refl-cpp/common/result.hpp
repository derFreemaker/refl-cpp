#pragma once

#include <optional>
#include <utility>

#include "refl-cpp/common/type_traits.hpp"
#include "refl-cpp/common/stack_tracing_error.hpp"

namespace ReflCpp {
namespace detail {
struct ErrorTag {};

inline constexpr ErrorTag Error {};

template <typename T_>
struct ResultBase {
private:
    using StoredT = T_;

    union {
        const ResultError m_Error;
        StoredT m_Value;
    };

    bool m_IsSuccess;

    void m_ThrowBadAccessException() const {
        throw std::runtime_error("Attempted to access value of an error Result. Error: " + this->Error().Str());
    }

public:
    ResultBase(ErrorTag, ResultError&& error)
        : m_Error(std::move(error)),
          m_IsSuccess(false) {}

    ResultBase(ErrorTag, const ResultError& error)
        : m_Error(error),
          m_IsSuccess(false) {}

    template <typename T2_>
    ResultBase(T2_&& value) noexcept // NOLINT(*-forwarding-reference-overload)
        : m_Value(std::forward<T2_>(value)),
          m_IsSuccess(true) {}

    //NOTE: we need this since it is not implicitly deleted
    ~ResultBase() {}

    [[nodiscard]]
    bool IsSuccess() const {
        return m_IsSuccess;
    }

    [[nodiscard]]
    bool IsError() const {
        return !m_IsSuccess;
    }

    [[nodiscard]]
    const ResultError& Error() const {
        if (IsSuccess()) {
            throw std::runtime_error("Attempted to access error of a success Result");
        }
        return m_Error;
    }

    StoredT& Value() & {
        if (this->IsError()) {
            this->m_ThrowBadAccessException();
        }
        return this->m_Value;
    }

    make_const<StoredT>& Value() const & {
        if (this->IsError()) {
            this->m_ThrowBadAccessException();
        }
        return this->m_Value;
    }

    StoredT& Value() && {
        if (this->IsError()) {
            this->m_ThrowBadAccessException();
        }
        return this->m_Value;
    }

    make_const<StoredT>& Value() const && {
        if (this->IsError()) {
            this->m_ThrowBadAccessException();
        }
        return this->m_Value;
    }
};

template <>
struct ResultBase<void> {
private:
    const std::optional<ResultError> m_Error;

public:
    ResultBase(ErrorTag, ResultError&& error) noexcept
        : m_Error(std::move(error)) {}

    ResultBase(ErrorTag, const ResultError& error) noexcept
        : m_Error(error) {}

    ResultBase() noexcept = default;

    [[nodiscard]]
    bool IsSuccess() const {
        return !m_Error.has_value();
    }

    [[nodiscard]]
    bool IsError() const {
        return m_Error.has_value();
    }

    [[nodiscard]]
    const ResultError& Error() const {
        if (IsSuccess()) {
            throw std::runtime_error("Attempted to access error of a success Result");
        }
        return m_Error.value();
    }
};
}

template <typename T_>
struct Result;

template <>
struct Result<void> : detail::ResultBase<void> {
    Result() noexcept = default;

#ifndef NDEBUG
    template <typename... Args>
    Result(detail::ErrorTag,
           const std::stacktrace&& stacktrace,
           const std::string_view& format, Args&&... args) noexcept
        : ResultBase(detail::Error, ResultError(
                         stacktrace,
                         format, std::forward<Args>(args)...)) {}
#endif

    template <typename... Args>
    Result(detail::ErrorTag,
           const std::string_view& format, Args&&... args) noexcept
        : ResultBase(detail::Error, ResultError(
                         format,
                         std::forward<Args>(args)...)) {}

#ifndef NDEBUG
    Result(detail::ErrorTag,
           const std::stacktrace&& stacktrace,
           const FormattedError& error) noexcept
        : ResultBase(detail::Error, ResultError(
                         stacktrace,
                         error)) {}
#endif

    Result(detail::ErrorTag, const ResultError& error) noexcept
        : ResultBase(detail::Error, error) {}
};

template <typename T_>
struct Result : detail::ResultBase<T_> {
#ifndef NDEBUG
    template <typename... Args>
    Result(detail::ErrorTag,
           const std::stacktrace&& stacktrace,
           const std::string_view& format, Args&&... args) noexcept
        : detail::ResultBase<T_>(detail::Error, ResultError(
                                     stacktrace,
                                     format, std::forward<Args>(args)...)) {}
#endif

    template <typename... Args>
    Result(detail::ErrorTag,
           const std::string_view& format, Args&&... args) noexcept
        : detail::ResultBase<T_>(detail::Error, ResultError(
                                     format,
                                     std::forward<Args>(args)...)) {}

#ifndef NDEBUG
    Result(detail::ErrorTag,
           const std::stacktrace&& stacktrace,
           const FormattedError& error)
        : detail::ResultBase<T_>(detail::Error, ResultError(
                                     stacktrace,
                                     error)) {}
#endif

    Result(detail::ErrorTag, const ResultError& error) noexcept
        : detail::ResultBase<T_>(detail::Error, error) {}

    template <typename T2_>
        requires (std::is_nothrow_convertible_v<T2_, T_>)
    Result(T2_&& value) noexcept
        : detail::ResultBase<T_>(std::forward<T2_>(value)) {}
};

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

#ifndef NDEBUG

#define RESULT_ERROR() \
    ::ReflCpp::detail::Error, std::stacktrace::current()

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
