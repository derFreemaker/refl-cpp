#pragma once
#include <string>

#include "fmt/base.h"
#include "fmt/format.h"

// template <typename T, typename E = FormattedError>
// class Result {
// private:
//     std::variant<T, E> data;
//     bool has_value_;
//
// public:
//     // Value constructor
//     template <typename U = T,
//               typename = std::enable_if_t<!std::is_same_v<std::decay_t<U>, Result>>>
//     Result(U&& value) : data(std::forward<U>(value)), has_value_(true) {}
//
//     // Direct error constructor
//     Result(E error) : data(std::move(error)), has_value_(false) {}
//
//     // Constructor for ErrorArgs syntax
//     template <typename... Args>
//     Result(ErrorArgs<Args...>&& error_args) : has_value_(false) {
//         auto format_error = [](fmt::format_string<Args...> fmt_str, Args&&... args) {
//             return FormattedError(fmt_str, std::forward<Args>(args)...);
//         };
//
//         data = std::apply([&](Args&&... args) {
//             return format_error(error_args.fmt_str, std::forward<decltype(args)>(args)...);
//         }, std::move(error_args.args));
//     }
//
//     // Status checking
//     bool has_value() const {
//         return has_value_;
//     }
//
//     bool has_error() const {
//         return !has_value_;
//     }
//
//     // Value access with safety checks
//     const T& value() const & {
//         if (!has_value_) {
//             throw std::runtime_error(fmt::format("Attempted to access value of an error Result. Error: {}",
//                                                  std::get<E>(data).what()));
//         }
//         return std::get<T>(data);
//     }
//
//     T&& value() && {
//         if (!has_value_) {
//             throw std::runtime_error(fmt::format("Attempted to access value of an error Result. Error: {}",
//                                                  std::get<E>(data).what()));
//         }
//         return std::move(std::get<T>(data));
//     }
//
//     // Error access with safety checks
//     const E& error() const & {
//         if (has_value_) {
//             throw std::runtime_error("Attempted to access error of a value Result");
//         }
//         return std::get<E>(data);
//     }
//
//     // Value or default fallback
//     T value_or(T default_value) const {
//         if (has_value_) {
//             return std::get<T>(data);
//         }
//         return default_value;
//     }
//
//     // Transform success value
//     template <typename Func>
//     auto map(Func&& func) const -> Result<decltype(func(std::declval<T>())), E> {
//         if (has_value_) {
//             return func(std::get<T>(data));
//         }
//         return std::get<E>(data);
//     }
//
//     // Transform error value
//     template <typename Func>
//     auto map_error(Func&& func) const -> Result<T, decltype(func(std::declval<E>()))> {
//         if (has_value_) {
//             return std::get<T>(data);
//         }
//         return func(std::get<E>(data));
//     }
//
//     // Boolean conversion for if checks
//     operator bool() const {
//         return has_value_;
//     }
//
//     // Monadic bind for chaining
//     template <typename Func>
//     auto and_then(Func&& func) const -> decltype(func(std::declval<T>())) {
//         using ReturnType = decltype(func(std::declval<T>()));
//
//         if (has_value_) {
//             return func(std::get<T>(data));
//         }
//
//         return ReturnType{std::get<E>(data)};
//     }
// };
//
// // Specialization for void value type
// template <typename E>
// class Result<void, E> {
// private:
//     std::optional<E> error_;
//
// public:
//     // Success constructor
//     Result() : error_(std::nullopt) {}
//
//     // Error constructor
//     Result(E error) : error_(std::move(error)) {}
//
//     // Constructor for ErrorArgs syntax
//     template <typename... Args>
//     Result(ErrorArgs<Args...>&& error_args) {
//         auto format_error = [](fmt::format_string<Args...> fmt_str, Args&&... args) {
//             return FormattedError(fmt_str, std::forward<decltype(args)>(args)...);
//         };
//
//         error_ = std::apply([&](Args&&... args) {
//             return format_error(error_args.fmt_str, std::forward<decltype(args)>(args)...);
//         }, std::move(error_args.args));
//     }
//
//     bool has_value() const {
//         return !error_.has_value();
//     }
//
//     bool has_error() const {
//         return error_.has_value();
//     }
//
//     // Error access with safety check
//     const E& error() const & {
//         if (!error_) {
//             throw std::runtime_error("Attempted to access error of a value Result");
//         }
//         return *error_;
//     }
//
//     // Boolean conversion for if checks
//     operator bool() const {
//         return !error_.has_value();
//     }
//
//     // Monadic bind for chaining
//     template <typename Func>
//     auto and_then(Func&& func) const -> decltype(func()) {
//         using ReturnType = decltype(func());
//
//         if (!error_) {
//             return func();
//         }
//
//         return ReturnType{*error_};
//     }
//
//     template <typename T, typename Func>
//     auto map(Func&& func) const -> Result<T, E> {
//         if (!error_) {
//             return func();
//         }
//         return *error_;
//     }
// };

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

inline constexpr detail::ErrorTag Error{};
inline constexpr detail::OkTag Ok{};

template <typename T_>
struct Result {
private:
    const bool m_IsSuccess;

    union {
        T_ Result;
        const FormattedError Error;
    } m_Data;

public:
    Result(detail::OkTag, const T_& result)
        : m_IsSuccess(true),
          m_Data(result) {}

    template <typename... Args>
    Result(detail::ErrorTag, fmt::format_string<Args...> fmt_str, Args&&... args)
        : m_IsSuccess(false),
          m_Data(FormattedError(fmt_str, args...)) {}

    [[nodiscard]]
    bool IsSuccess() const {
        return m_IsSuccess;
    }

    [[nodiscard]]
    bool IsError() const {
        return !m_IsSuccess;
    }

    const T_& Value() const & {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return m_Data.Result;
    }

    T_&& Value() && {
        if (IsError()) {
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return std::move(m_Data.Result);
    }

    operator const T_&() const & {
        return Value();
    }

    operator T_&&() && {
        return Value();
    }

    [[nodiscard]]
    const FormattedError& Error() const & {
        if (IsSuccess()) {
            throw std::runtime_error("Attempted to access error of a success Result");
        }
        return m_Data.Error;
    }

    operator const FormattedError&() const & {
        return Error();
    }
};

struct Result<void> {
private:
    const std::optional<FormattedError> m_Error;

public:
    Result(detail::OkTag) {}

    Result(detail::ErrorTag, const FormattedError& error)
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
            throw std::runtime_error("Attempted to access value of an error Result.");
        }
        return *m_Error;
    }

    operator const FormattedError&() const & {
        return Error();
    }
};
}
