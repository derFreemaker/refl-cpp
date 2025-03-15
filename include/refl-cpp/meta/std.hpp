#pragma once

#include <optional>
#include <vector>

#include "refl-cpp/declare_reflect.hpp"
#include "refl-cpp/reflect_printer.hpp"

template <typename T>
struct ReflCpp::ReflectData<std::optional<T>> {
    static TypeData Create() {
        return {
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
    static TypeData Create() {
        return {
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

template <>
struct ReflCpp::ReflectData<std::nullptr_t> {
    static TypeData Create() {
        return {
            .name = "Null Pointer",
            ._namespace = "std",
        };
    }
};

template <>
struct ReflCpp::ReflectPrinter<std::nullptr_t> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "nullptr";
    }
};
