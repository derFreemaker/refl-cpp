#pragma once

#include <string>

namespace ReflCpp::Common {
struct ListSeperator {
private:
    bool first_ = true;
    std::string sep_ = ", ";

public:
    ListSeperator() = default;

    ListSeperator(const std::string& sep)
        : sep_(sep) {}

    const char* Get() {
        if (first_) {
            first_ = false;
            return "";
        }

        return sep_.c_str();
    }

    operator std::string() {
        return Get();
    }

    operator const char*() {
        return Get();
    }
};
}
