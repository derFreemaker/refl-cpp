#pragma once

#include "refl-cpp/declare_reflect.hpp"

template <typename T_>
struct ReflCpp::ReflectData<T_[]> {
    static TypeData Create() {
        return {
            .name = "Array",
            .inners = ReflectID<T_>(),
            .flags = TypeFlags::IsArray,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_[]> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().get().Print(stream);
        stream << "[]";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_*> {
    static TypeData Create() {
        return {
            .name = "Pointer",
            .inners = {
                ReflectID<T_>()
            },
            .flags = TypeFlags::IsPointer,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_*> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().get().Print(stream);
        stream << "*";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_* const> {
    static TypeData Create() {
        return {
            .name = "Constant Pointer",
            .inners = {
                ReflectID<T_>()
            },
            .flags = TypeFlags::IsPointer | TypeFlags::IsConst,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_* const> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().get().Print(stream);
        stream << "* const";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_&> {
    static TypeData Create() {
        return {
            .name = "LValue Reference",
            .inners = {
                ReflectID<T_>()
            },
            .flags = TypeFlags::IsLValueReference,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_&> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().get().Print(stream);
        stream << "&";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_&&> {
    static TypeData Create() {
        return {
            .name = "RValue Reference",
            .inners = {
                ReflectID<T_>()
            },
            .flags = TypeFlags::IsRValueReference,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_&&> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().get().Print(stream);
        stream << "&&";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<const T_> {
    static TypeData Create() {
        return {
            .name = "Constant",
            .inners = {
                ReflectID<T_>()
            },
            .flags = TypeFlags::IsConst,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<const T_> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "const ";
        type.GetInner(0).Value().get().Print(stream);
    }
};

template <typename T_>
struct ReflCpp::ReflectData<volatile T_> {
    static TypeData Create() {
        return {
            .name = "Volatile",
            .inners = {
                ReflectID<T_>()
            },
            .flags = TypeFlags::IsVolatile,
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<volatile T_> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "volatile ";
        type.GetInner(0).Value().get().Print(stream);
    }
};
