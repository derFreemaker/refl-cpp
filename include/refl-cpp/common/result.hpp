#pragma once

#include <optional>
#include <utility>

#include "refl-cpp/common/reference_wrapper.hpp"
#include "refl-cpp/common/type_traits.hpp"
#include "refl-cpp/common/stack_tracing_error.hpp"

namespace ReflCpp {
namespace detail {
struct ErrorTag {};

struct PassErrorTag {};

inline constexpr ErrorTag Error{};
inline constexpr PassErrorTag PassError{};

template <typename T_>
struct ResultBase {
    using StoredT = T_;
    using StoredReturnT = std::conditional_t<std::is_reference_v<StoredT>,
                                             StoredT,
                                             make_lvalue_reference_t<StoredT>>;

private:
    union {
        const ResultError m_Error;
        std::conditional_t<std::is_reference_v<StoredT>,
                           reference_wrapper<StoredT>,
                           StoredT> m_Value;
    };

    bool _hasError;

    void m_ThrowBadAccessException() const {
        throw std::runtime_error("Attempted to access value of an error Result. Error: " + this->Error().Str());
    }

public:
    ResultBase(ErrorTag, ResultError&& error)
        : m_Error(std::move(error)),
          _hasError(true) {}

    ResultBase(ErrorTag, const ResultError& error)
        : m_Error(error),
          _hasError(true) {}

    template <typename T2_>
    ResultBase(T2_&& value) noexcept // NOLINT(*-forwarding-reference-overload)
        : m_Value(std::forward<T2_>(value)),
          _hasError(false) {}

    //NOTE: we need this deconstructor since it is implicitly deleted
    ~ResultBase() {}

    [[nodiscard]]
    bool HasError() const {
        return _hasError;
    }

    [[nodiscard]]
    const ResultError& Error() const {
        if (!HasError()) {
            throw std::runtime_error("Attempted to access error of a success Result");
        }
        return m_Error;
    }


    StoredReturnT Value() & {
        if (this->HasError()) {
            this->m_ThrowBadAccessException();
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return this->m_Value.get();
        }
        if constexpr (std::is_rvalue_reference_v<StoredT>) {
            return std::forward<std::remove_reference_t<StoredT>>(this->m_Value.get());
        }
        else {
            return this->m_Value;
        }
    }

    make_const_t<StoredReturnT> Value() const & {
        if (this->HasError()) {
            this->m_ThrowBadAccessException();
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return this->m_Value.get();
        }
        if constexpr (std::is_rvalue_reference_v<StoredT>) {
            return std::forward<std::remove_reference_t<StoredT>>(this->m_Value.get());
        }
        else {
            return this->m_Value;
        }
    }

    StoredReturnT Value() && {
        if (this->HasError()) {
            this->m_ThrowBadAccessException();
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return this->m_Value.get();
        }
        if constexpr (std::is_rvalue_reference_v<StoredT>) {
            return std::forward<std::remove_reference_t<StoredT>>(this->m_Value.get());
        }
        else {
            return this->m_Value;
        }
    }

    make_const_t<StoredReturnT> Value() const && {
        if (this->HasError()) {
            this->m_ThrowBadAccessException();
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return this->m_Value.get();
        }
        if constexpr (std::is_rvalue_reference_v<StoredT>) {
            return std::forward<std::remove_reference_t<StoredT>>(this->m_Value.get());
        }
        else {
            return this->m_Value;
        }
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
    bool HasError() const {
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

    template <typename OtherT_>
    Result(detail::PassErrorTag, const Result<OtherT_>& result) noexcept
        : ResultBase(detail::Error, result.Error()) {}
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

    template <typename OtherT_>
    Result(detail::PassErrorTag, const Result<OtherT_>& result) noexcept
        : detail::ResultBase<T_>(detail::Error, result.Error()) {}

    template <typename T2_>
        requires (!std::is_same_v<T2_, T_> && std::is_nothrow_convertible_v<T2_, T_>)
    Result(T2_&& value) noexcept
        : detail::ResultBase<T_>(std::forward<T2_>(value)) {}

    Result(T_&& value) noexcept
        : detail::ResultBase<T_>(std::forward<T_>(value)) {}
};

namespace detail {
inline void TryHelper(const Result<void>* _) {}

template <typename T_>
typename Result<T_>::StoredReturnT TryHelper(Result<T_>* result) {
    return result->Value();
}

template <typename T_>
typename Result<T_>::StoredReturnT TryHelper(const Result<T_>* result) {
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

#define RESULT_PASS_ERROR() \
    ::ReflCpp::detail::PassError

#ifndef NDEBUG

#define RESULT_ERROR() \
    ::ReflCpp::detail::Error, std::stacktrace::current()

#define TRY(...) \
    (::ReflCpp::detail::TryHelper( \
        ({ \
            auto _result = (__VA_ARGS__); \
            if (_result.HasError()) { \
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
            if (_result.HasError()) { \
                return { ::ReflCpp::detail::Error, _result.Error() }; \
            } \
            &_result; \
        }) \
    ))

#endif
}
