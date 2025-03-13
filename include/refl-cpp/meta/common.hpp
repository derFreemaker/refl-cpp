#pragma once

#include "refl-cpp/declare_reflect.hpp"

template <typename T>
struct ReflCpp::ReflectData<T[]> {
    static TypeData Create() {
        return {
            .name = "Array",
            .inner = ReflectID<T>(),
            .is_array = true,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<T[]> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner().Print(stream);
        stream << "[]";
    }
};

template <typename T>
struct ReflCpp::ReflectData<T*> {
    static TypeData Create() {
        return {
            .name = "Pointer",
            .inner = ReflectID<T>(),
            .is_pointer = true,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<T*> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner().Print(stream);
        stream << "*";
    }
};

template <typename T>
struct ReflCpp::ReflectData<T&> {
    static TypeData Create() {
        return {
            .name = "Reference",
            .inner = ReflectID<T>(),
            .is_reference = true,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<T&> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner().Print(stream);
        stream << "&";
    }
};

template <typename T>
struct ReflCpp::ReflectData<const T> {
    static TypeData Create() {
        return {
            .name = "Constant",
            .inner = ReflectID<T>(),
            .is_const = true,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<const T> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "const ";
        type.GetInner().Print(stream);
    }
};

template <typename T>
struct ReflCpp::ReflectData<volatile T> {
    static TypeData Create() {
        return {
            .name = "Volatile",
            .inner = ReflectID<T>(),
            .is_volatile = true,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<volatile T> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "volatile ";
        type.GetInner().Print(stream);
    }
};