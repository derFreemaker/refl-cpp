#pragma once

#include <variant>
#include <optional>
#include <type_traits>
#include <utility>
#include <tuple>

#include <fmt/format.h>

namespace ReflCpp {

template <typename... Args>
struct ErrorArgs;

// Tag struct for error creation
struct Error {
    template <typename... Args>
    auto operator()(fmt::format_string<Args...> fmt_str, Args&&... args) const {
        return ErrorArgs{fmt_str, std::forward<Args>(args)...};
    }
};

// Error type that can be constructed from a format string and arguments
class FormattedError {
private:
    std::string message;

public:
    // Constructor for direct string
    explicit FormattedError(std::string msg) : message(std::move(msg)) {}

    // Constructor for format string and arguments
    template <typename... Args>
    FormattedError(fmt::format_string<Args...> fmt_str, Args&&... args)
        : message(fmt::format(fmt_str, std::forward<Args>(args)...)) {}

    // Getter for the error message
    const std::string& what() const {
        return message;
    }

    // Conversion to string
    operator std::string() const {
        return message;
    }

    // String conversion for output streams
    friend std::ostream& operator<<(std::ostream& os, const FormattedError& error) {
        return os << error.message;
    }
};

// Helper struct for error arguments
template <typename... Args>
struct ErrorArgs {
    fmt::format_string<Args...> fmt_str;
    std::tuple<Args...> args;

    ErrorArgs(fmt::format_string<Args...> fmt, Args&&... arguments)
        : fmt_str(fmt), args(std::forward<Args>(arguments)...) {}
};

// Deduction guide
template <typename... Args>
ErrorArgs(fmt::format_string<Args...>, Args&&...) -> ErrorArgs<Args...>;

// Inline constant for error creation
inline constexpr Error error{};

template <typename T, typename E = FormattedError>
class Result {
private:
    std::variant<T, E> data;
    bool has_value_;

public:
    // Value constructor
    template <typename U = T,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<U>, Result>>>
    Result(U&& value) : data(std::forward<U>(value)), has_value_(true) {}

    // Direct error constructor
    Result(E error) : data(std::move(error)), has_value_(false) {}

    // Constructor for ErrorArgs syntax
    template <typename... Args>
    Result(ErrorArgs<Args...>&& error_args) : has_value_(false) {
        auto format_error = [](fmt::format_string<Args...> fmt_str, Args&&... args) {
            return FormattedError(fmt_str, std::forward<Args>(args)...);
        };

        data = std::apply([&](Args&&... args) {
            return format_error(error_args.fmt_str, std::forward<decltype(args)>(args)...);
        }, std::move(error_args.args));
    }

    // Status checking
    bool has_value() const {
        return has_value_;
    }

    bool has_error() const {
        return !has_value_;
    }

    // Value access with safety checks
    const T& value() const & {
        if (!has_value_) {
            throw std::runtime_error(fmt::format("Attempted to access value of an error Result. Error: {}",
                                                 std::get<E>(data).what()));
        }
        return std::get<T>(data);
    }

    T&& value() && {
        if (!has_value_) {
            throw std::runtime_error(fmt::format("Attempted to access value of an error Result. Error: {}",
                                                 std::get<E>(data).what()));
        }
        return std::move(std::get<T>(data));
    }

    // Error access with safety checks
    const E& error() const & {
        if (has_value_) {
            throw std::runtime_error("Attempted to access error of a value Result");
        }
        return std::get<E>(data);
    }

    // Value or default fallback
    T value_or(T default_value) const {
        if (has_value_) {
            return std::get<T>(data);
        }
        return default_value;
    }

    // Transform success value
    template <typename Func>
    auto map(Func&& func) const -> Result<decltype(func(std::declval<T>())), E> {
        if (has_value_) {
            return func(std::get<T>(data));
        }
        return std::get<E>(data);
    }

    // Transform error value
    template <typename Func>
    auto map_error(Func&& func) const -> Result<T, decltype(func(std::declval<E>()))> {
        if (has_value_) {
            return std::get<T>(data);
        }
        return func(std::get<E>(data));
    }

    // Boolean conversion for if checks
    operator bool() const {
        return has_value_;
    }

    // Monadic bind for chaining
    template <typename Func>
    auto and_then(Func&& func) const -> decltype(func(std::declval<T>())) {
        using ReturnType = decltype(func(std::declval<T>()));

        if (has_value_) {
            return func(std::get<T>(data));
        }

        return ReturnType{std::get<E>(data)};
    }
};

// Specialization for void value type
template <typename E>
class Result<void, E> {
private:
    std::optional<E> error_;

public:
    // Success constructor
    Result() : error_(std::nullopt) {}

    // Error constructor
    Result(E error) : error_(std::move(error)) {}

    // Constructor for ErrorArgs syntax
    template <typename... Args>
    Result(ErrorArgs<Args...>&& error_args) {
        auto format_error = [](fmt::format_string<Args...> fmt_str, Args&&... args) {
            return FormattedError(fmt_str, std::forward<decltype(args)>(args)...);
        };

        error_ = std::apply([&](Args&&... args) {
            return format_error(error_args.fmt_str, std::forward<decltype(args)>(args)...);
        }, std::move(error_args.args));
    }

    bool has_value() const {
        return !error_.has_value();
    }

    bool has_error() const {
        return error_.has_value();
    }

    // Error access with safety check
    const E& error() const & {
        if (!error_) {
            throw std::runtime_error("Attempted to access error of a value Result");
        }
        return *error_;
    }

    // Boolean conversion for if checks
    operator bool() const {
        return !error_.has_value();
    }

    // Monadic bind for chaining
    template <typename Func>
    auto and_then(Func&& func) const -> decltype(func()) {
        using ReturnType = decltype(func());

        if (!error_) {
            return func();
        }

        return ReturnType{*error_};
    }

    template <typename T, typename Func>
    auto map(Func&& func) const -> Result<T, E> {
        if (!error_) {
            return func();
        }
        return *error_;
    }
};
}
