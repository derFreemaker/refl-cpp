#pragma once

#include "refl-cpp/declare_reflect.hpp"

template <typename T_>
struct ReflCpp::ReflectData<T_[]> {
    static TypeData Create() {
        return {
            .name = "Array",
            .inner = ReflectID<T_>(),
            .is_array = true,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_[]> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner().Print(stream);
        stream << "[]";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_*> {
    static TypeData Create() {
        return {
            .name = "Pointer",
            .inner = ReflectID<T_>(),
            .is_pointer = true,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_*> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner().Print(stream);
        stream << "*";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_&> {
    static TypeData Create() {
        return {
            .name = "Reference",
            .inner = ReflectID<T_>(),
            .is_reference = true,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_&> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner().Print(stream);
        stream << "&";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<const T_> {
    static TypeData Create() {
        return {
            .name = "Constant",
            .inner = ReflectID<T_>(),
            .is_const = true,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<const T_> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "const ";
        type.GetInner().Print(stream);
    }
};

template <typename T_>
struct ReflCpp::ReflectData<volatile T_> {
    static TypeData Create() {
        return {
            .name = "Volatile",
            .inner = ReflectID<T_>(),
            .is_volatile = true,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<volatile T_> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "volatile ";
        type.GetInner().Print(stream);
    }
};
