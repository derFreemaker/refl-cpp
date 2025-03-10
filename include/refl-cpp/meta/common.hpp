#pragma once

#include "refl-cpp/reflect.hpp"

template <typename T>
struct ReflCpp::ReflectData<T[]> {
    static ReflectTypeData Create() {
        return {
            .name = "Array",
            .inner = Reflect<T>(),
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
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Pointer",
            .inner = Reflect<T>(),
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
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Reference",
            .inner = Reflect<T>(),
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
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Constant",
            .inner = Reflect<T>(),
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
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Volatile",
            .inner = Reflect<T>(),
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