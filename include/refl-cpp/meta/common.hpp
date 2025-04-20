#pragma once

#include "refl-cpp/declare_reflect.hpp"

template <typename T>
struct ReflCpp::ReflectData<T[]> {
    static TypeData Create() {
        return TypeData{
            .name = "Array",
            .inners = ReflectID<T>(),
            .flags = TypeFlags::IsArray,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<T[]> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Print(stream);
        stream << "[]";
    }
};

template <typename T>
struct ReflCpp::ReflectData<T*> {
    static TypeData Create() {
        return TypeData{
            .name = "Pointer",
            .inners = {
                ReflectID<T>()
            },
            .flags = TypeFlags::IsPointer,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<T*> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Print(stream);
        stream << "*";
    }
};

template <typename T>
struct ReflCpp::ReflectData<T* const> {
    static TypeData Create() {
        return TypeData{
            .name = "Constant Pointer",
            .inners = {
                ReflectID<T>()
            },
            .flags = TypeFlags::IsPointer | TypeFlags::IsConst,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<T* const> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Print(stream);
        stream << "* const";
    }
};

template <typename T>
struct ReflCpp::ReflectData<T&> {
    static TypeData Create() {
        return TypeData{
            .name = "LValue Reference",
            .inners = {
                ReflectID<T>()
            },
            .flags = TypeFlags::IsLValueReference,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<T&> {
    static void Print(std::ostream& stream, const Type& type) noexcept {
        type.GetInner(0).Print(stream);
        stream << "&";
    }
};

template <typename T>
struct ReflCpp::ReflectData<T&&> {
    static TypeData Create() noexcept {
        return TypeData{
            .name = "RValue Reference",
            .inners = {
                ReflectID<T>()
            },
            .flags = TypeFlags::IsRValueReference,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<T&&> {
    static void Print(std::ostream& stream, const Type& type) noexcept {
        type.GetInner(0).Print(stream);
        stream << "&&";
    }
};

template <typename T>
struct ReflCpp::ReflectData<const T> {
    static TypeData Create() noexcept {
        return TypeData{
            .name = "Constant",
            .inners = {
                ReflectID<T>()
            },
            .flags = TypeFlags::IsConst,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<const T> {
    static void Print(std::ostream& stream, const Type& type) noexcept {
        stream << "const ";
        type.GetInner(0).Print(stream);
    }
};

template <typename T>
struct ReflCpp::ReflectData<volatile T> {
    static TypeData Create() noexcept {
        return TypeData{
            .name = "Volatile",
            .inners = {
                ReflectID<T>()
            },
            .flags = TypeFlags::IsVolatile,
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<volatile T> {
    static void Print(std::ostream& stream, const Type& type) noexcept {
        stream << "volatile ";
        type.GetInner(0).Print(stream);
    }
};
