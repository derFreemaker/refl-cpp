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
    using ReturnT = std::conditional_t<std::is_reference_v<StoredT>,
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
    ResultBase(ErrorTag, ResultError&& error) noexcept
        : error_(error),
          hasError_(true) {}

    ResultBase(ErrorTag, const ResultError& error) noexcept
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
    ~ResultBase() noexcept {}

    ResultBase(const ResultBase& other) noexcept {
        this->~ResultBase();
        this->hasError_ = other.hasError_;
        if (this->hasError_) {
            this->error_ = other.error_;
        }
        else {
            this->value_ = std::move(other.value_);
        }
    }

    ResultBase& operator=(const ResultBase& other) noexcept {
        if (this == &other) {
            return *this;
        }

        this->~ResultBase();
        this->hasError_ = other.hasError_;
        if (this->hasError_) {
            this->error_ = other.error_;
        }
        else {
            this->value_ = other.value_;
        }

        return *this;
    }

    [[nodiscard]]
    bool HasError() const noexcept {
        return hasError_;
    }

    [[nodiscard]]
    const ResultError& Error() const {
        if (!HasError()) {
            ResultThrowBadErrorAccessException();
        }
        return error_;
    }


    ReturnT Value() & {
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

    make_const_t<ReturnT> Value() const & {
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

    ReturnT Value() && {
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

    make_const_t<ReturnT> Value() const && {
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
    std::optional<ResultError> error_;

public:
    ResultBase(ErrorTag, ResultError&& error) noexcept
        : error_(std::move(error)) {}

    ResultBase(ErrorTag, const ResultError& error) noexcept
        : error_(error) {}

    ResultBase() noexcept = default;

    ResultBase(const ResultBase& other) noexcept {
        this->~ResultBase();
        this->error_ = other.Error();
    }

    ResultBase& operator=(const ResultBase& other) noexcept {
        if (this == &other) {
            return *this;
        }

        this->~ResultBase();
        this->error_ = other.Error();

        return *this;
    }

    [[nodiscard]]
    bool HasError() const noexcept {
        return error_.has_value();
    }

    [[nodiscard]]
    const ResultError& Error() const {
        if (!HasError()) {
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
           const FormattedError& error) noexcept
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
template <typename T_>
T_& TryResultStorage() {
    static thread_local std::aligned_storage_t<sizeof(T_), alignof(T_)> result_storage;
    return *reinterpret_cast<T_*>(&result_storage);
}

inline void TryHelper(Result<void>*) noexcept {}
inline void TryHelper(const Result<void>*) noexcept {}

template <typename T_>
[[nodiscard]]
typename Result<T_>::ReturnT TryHelper(Result<T_>*) noexcept {
    // result->Value could throw an exception but since we only use this
    // function normally after checking if result has an error we should be good
    return TryResultStorage<Result<T_>>().Value();
}

template <typename T_>
[[nodiscard]]
typename Result<T_>::ReturnT TryHelper(const Result<T_>*) noexcept {
    // result->Value could throw an exception but since we only use this
    // function normally after checking if result has an error we should be good
    return TryResultStorage<Result<T_>>().Value();
}
}

//TODO: maybe include the expression as well for debug purposes?
//TODO: improve debug information using std::stacktrace is unnecessary long

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
#define TRY_IMPL(expr, ...) \
    (::ReflCpp::detail::TryHelper( \
        ({ \
            using __result_type__ = decltype(expr); \
            __result_type__* __result__ = nullptr; \
            { \
                void* __result_ptr__ = reinterpret_cast<void*>(&::ReflCpp::detail::TryResultStorage<__result_type__>()); \
                new (__result_ptr__) __result_type__(expr); \
                __result__ = reinterpret_cast<__result_type__*>(__result_ptr__); \
            } \
            if (__result__->HasError()) { \
                __VA_ARGS__ \
            } \
            __result__; \
        }) \
    ))

#define TRY(...) \
    TRY_IMPL((__VA_ARGS__), return __result__->Error();)

}
