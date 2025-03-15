#pragma once

#include <string>

namespace ReflCpp::Common {

struct ListSeperator {
private:
    bool m_First = true;
    std::string m_Sep = ", ";

public:
    ListSeperator() = default;
    ListSeperator(const std::string& sep)
        : m_Sep(sep) {}

    const char* Get() {
        if (m_First) {
            m_First = false;
            return "";
        }

        return m_Sep.c_str();
    }
    
    operator std::string() {
        return Get();
    }
    operator const char*() {
        return Get();
    }
};

}