#pragma once

#include <sstream>
#include <string>

#include <fmt/format.h>

namespace ReflCpp {

struct FormattedError {
protected:
    std::string m_Message;

public:
    explicit FormattedError(const std::string& message)
        : m_Message(message) {}

    template <typename... Args>
    FormattedError(const std::string_view& format, Args&&... args)
        : m_Message(fmt::format(fmt::runtime(format), std::forward<Args>(args)...)) {}

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

}
