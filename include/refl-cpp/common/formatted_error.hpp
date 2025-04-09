#pragma once

#include <sstream>
#include <string>

#include <fmt/format.h>

#include "refl-cpp/common/result_tags.hpp"

namespace ReflCpp {

template <typename T_>
struct Result;

struct FormattedError {
private:
    const std::string msg_;

public:
    explicit FormattedError(const std::string& msg)
        : msg_(msg) {}

    template <typename... Args>
    FormattedError(const std::string_view& format, Args&&... args)
        : msg_(fmt::format(fmt::runtime(format), std::forward<Args>(args)...)) {}

    [[nodiscard]]
    const std::string& Message() const {
        return msg_;
    }

    void Str(std::ostream& stream) const {
        stream << Message();
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

    template <typename T_>
    operator Result<T_>() const {
        return Result<T_>(detail::Error, *this);
    }
};
}
