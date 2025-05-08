#pragma once

#include <string>

namespace ReflCpp::Common {
struct ListSeperator {
private:
    bool first_ = true;
    std::string sep_ = ", ";

public:
    ListSeperator() noexcept = default;

    ListSeperator(const std::string& sep) noexcept
        : sep_(sep) {}

    const char* get() noexcept {
        if (first_) {
            first_ = false;
            return "";
        }

        return sep_.c_str();
    }

    operator const char*() noexcept {
        return get();
    }
};
}
