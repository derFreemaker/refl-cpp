#pragma once

#include <optional>
#include <vector>

#include "refl-cpp/reflect.hpp"

template <typename T>
struct ReflCpp::ReflectData<std::optional<T>> {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "optional",
            ._namespace = "std",
            .inner = ReflectID<T>(),
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<std::optional<T>> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::optional<";
        type.GetInner().Print(stream);
        stream << ">";
    }
};

template <typename T>
struct ReflCpp::ReflectData<std::vector<T>> {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "vector",
            ._namespace = "std",
            .inner = ReflectID<T>(),
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<std::vector<T>> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::vector<";
        type.GetInner().Print(stream);
        stream << ">";
    }
};