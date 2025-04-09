#pragma once

#include <optional>
#include <utility>

#include "refl-cpp/common/result_tags.hpp"
#include "refl-cpp/common/reference_wrapper.hpp"
#include "refl-cpp/common/type_traits.hpp"
#include "refl-cpp/common/stack_tracing_error.hpp"

namespace ReflCpp {
namespace detail {
inline void ResultThrowBadErrorAccessException() {
    throw std::runtime_error("Attempted to access error of a success Result");
}

template <typename T_>
struct ResultBase {
    using StoredT = T_;
    using StoringT = std::conditional_t<std::is_reference_v<StoredT>,
                           reference_wrapper<StoredT>,
                           StoredT>;
    using StoredReturnT = std::conditional_t<std::is_reference_v<StoredT>,
                                             StoredT,
                                             make_lvalue_reference_t<StoredT>>;

private:
    union {
        ResultError error_;
        StoringT value_;
    };

    bool hasError_;

    void m_ThrowBadValueAccessException() const {
        throw std::runtime_error("Attempted to access value of an error Result. Error: " + this->Error().Str());
    }

public:
    ResultBase(ErrorTag, ResultError&& error)
        : error_(error),
          hasError_(true) {}

    ResultBase(ErrorTag, const ResultError& error)
        : error_(error),
          hasError_(true) {}

    ResultBase(T_&& value) noexcept // NOLINT(*-forwarding-reference-overload)
        : value_(std::forward<T_>(value)),
          hasError_(false) {}
    
    template <typename T2_>
        requires (!std::is_same_v<T2_, T_> && std::is_nothrow_convertible_v<T2_, T_>)
    ResultBase(T2_&& value) noexcept // NOLINT(*-forwarding-reference-overload)
        : value_(std::forward<T2_>(value)),
          hasError_(false) {}

    //NOTE: we need this deconstructor since it is implicitly deleted
    ~ResultBase() {}

    ResultBase(const ResultBase& other) noexcept
        : hasError_(other.hasError_) {
        hasError_ = other.hasError_;
        if (hasError_) {
            error_ = other.error_;
        } else {
            value_ = other.value_;
        }
    }
    
    [[nodiscard]]
    bool HasError() const {
        return hasError_;
    }

    [[nodiscard]]
    const ResultError& Error() const {
        if (!HasError()) {
            ResultThrowBadErrorAccessException();
        }
        return error_;
    }


    StoredReturnT Value() & {
        if (this->HasError()) {
            this->m_ThrowBadValueAccessException();
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return this->value_.get();
        }
        if constexpr (std::is_rvalue_reference_v<StoredT>) {
            return std::forward<std::remove_reference_t<StoredT>>(this->value_.get());
        }
        else {
            return this->value_;
        }
    }

    make_const_t<StoredReturnT> Value() const & {
        if (this->HasError()) {
            this->m_ThrowBadValueAccessException();
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return this->value_.get();
        }
        if constexpr (std::is_rvalue_reference_v<StoredT>) {
            return std::forward<std::remove_reference_t<StoredT>>(this->value_.get());
        }
        else {
            return this->value_;
        }
    }

    StoredReturnT Value() && {
        if (this->HasError()) {
            this->m_ThrowBadValueAccessException();
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return this->value_.get();
        }
        if constexpr (std::is_rvalue_reference_v<StoredT>) {
            return std::forward<std::remove_reference_t<StoredT>>(this->value_.get());
        }
        else {
            return this->value_;
        }
    }

    make_const_t<StoredReturnT> Value() const && {
        if (this->HasError()) {
            this->m_ThrowBadValueAccessException();
        }
        if constexpr (std::is_lvalue_reference_v<StoredT>) {
            return this->value_.get();
        }
        if constexpr (std::is_rvalue_reference_v<StoredT>) {
            return std::forward<std::remove_reference_t<StoredT>>(this->value_.get());
        }
        else {
            return this->value_;
        }
    }
};

template <>
struct ResultBase<void> {
private:
    const std::optional<ResultError> error_;

public:
    ResultBase(ErrorTag, ResultError&& error) noexcept
        : error_(std::move(error)) {}

    ResultBase(ErrorTag, const ResultError& error) noexcept
        : error_(error) {}

    ResultBase() noexcept = default;

    [[nodiscard]]
    bool IsSuccess() const {
        return !error_.has_value();
    }

    [[nodiscard]]
    bool HasError() const {
        return error_.has_value();
    }

    [[nodiscard]]
    const ResultError& Error() const {
        if (IsSuccess()) {
            ResultThrowBadErrorAccessException();
        }
        return error_.value();
    }
};
}

template <typename T_>
struct Result;

template <>
struct Result<void> : detail::ResultBase<void> {
    Result() noexcept = default;

#if REFLCPP_ENABLE_STACK_TRACING_ERROR == 1
    template <typename... Args>
    Result(detail::ErrorTag,
           const std::stacktrace&& stacktrace,
           const std::string_view& format, Args&&... args) noexcept
        : ResultBase(detail::Error, ResultError(
                         stacktrace,
                         format, std::forward<Args>(args)...)) {}

    Result(detail::ErrorTag,
           const std::stacktrace&& stacktrace,
           const FormattedError& error) noexcept
        : ResultBase(detail::Error, ResultError(
                         stacktrace,
                         error)) {}
#else
    template <typename... Args>
    Result(detail::ErrorTag,
           const std::string_view& format, Args&&... args) noexcept
        : ResultBase(detail::Error, ResultError(
                         format,
                         std::forward<Args>(args)...)) {}
#endif

    Result(detail::ErrorTag, const ResultError& error) noexcept
        : ResultBase(detail::Error, error) {}
};

template <typename T_>
struct Result : detail::ResultBase<T_> {
#if REFLCPP_ENABLE_STACK_TRACING_ERROR == 1
    template <typename... Args>
    Result(detail::ErrorTag,
           const std::stacktrace&& stacktrace,
           const std::string_view& format, Args&&... args) noexcept
        : detail::ResultBase<T_>(detail::Error, ResultError(
                                     stacktrace,
                                     format, std::forward<Args>(args)...)) {}

    Result(detail::ErrorTag,
           const std::stacktrace&& stacktrace,
           const FormattedError& error)
        : detail::ResultBase<T_>(detail::Error, ResultError(
                                     stacktrace,
                                     error)) {}


#else
    template <typename... Args>
    Result(detail::ErrorTag,
           const std::string_view& format, Args&&... args) noexcept
        : detail::ResultBase<T_>(detail::Error, ResultError(
                                     format,
                                     std::forward<Args>(args)...)) {}
#endif

    Result(detail::ErrorTag, const ResultError& error) noexcept
        : detail::ResultBase<T_>(detail::Error, error) {}

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
[[nodiscard]]
typename Result<T_>::StoredReturnT TryHelper(Result<T_>* result) {
    return result->Value();
}

template <typename T_>
[[nodiscard]]
typename Result<T_>::StoredReturnT TryHelper(const Result<T_>* result) {
    return result->Value();
}
}

//TODO: maybe include the expression as well for debug purposes?
//TODO: improve debug information using std::stacktrace is unnecessary long
//TODO: create TRY macro for functions that don't return a result on error,
// with custom error handling and clean syntax

// The TRY macro uses a little bit of a lifetime hack,
// since we use '_result' after its scope through a pointer.
// This only works because '_result' lives in the enclosing
// function stack.

#if REFLCPP_ENABLE_STACK_TRACING_ERROR == 1

#define RESULT_ERROR() \
    ::ReflCpp::detail::Error, std::stacktrace::current()

#else

#define RESULT_ERROR() \
    ::ReflCpp::detail::Error

#endif

/// the 'expr' parameter has to be in parentheses for this to work!
/// otherwise we get macro comma problems
/// '__result__' is the variable name of the result
#define CUSTOM_TRY(expr, on_error) \
    (::ReflCpp::detail::TryHelper( \
        ({ \
            auto __result__ = (expr); \
            if (__result__.HasError()) { \
                on_error \
            } \
            &__result__; \
        }) \
    ))

#define TRY(...) \
    CUSTOM_TRY((__VA_ARGS__), return __result__.Error();)

}
