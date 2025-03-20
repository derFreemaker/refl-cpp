#pragma once

#include <optional>
#include <vector>

#include "refl-cpp/declare_reflect.hpp"
#include "refl-cpp/reflect_printer.hpp"

template <typename T_>
struct ReflCpp::ReflectData<std::optional<T_>> {
    static TypeData Create() {
        return {
            .name = "optional",
            ._namespace = "std",
            .inners = {
                ReflectID<T_>()
            },
            
            //TODO: take care of lvalue and rvalue references
            
            // .methods = {
            //     MethodData<const T_& (std::optional<T_>::*)() const&> {
            //         .ptr = &std::optional<T_>::value,
            //         .name = "value",
            //     },
            //     MethodData<T_& (std::optional<T_>::*)() &> {
            //         .ptr = &std::optional<T_>::value,
            //         .name = "value",
            //     },
            //     MethodData<const T_&& (std::optional<T_>::*)() const&&> {
            //         .ptr = &std::optional<T_>::value,
            //         .name = "value",
            //     },
            //     MethodData<T_&& (std::optional<T_>::*)() &&> {
            //         .ptr = &std::optional<T_>::value,
            //         .name = "value",
            //     },
            // }
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<std::optional<T>> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::optional<";
        type.GetInner(0).Value().get().Print(stream);
        stream << ">";
    }
};

template <typename T>
struct ReflCpp::ReflectData<std::vector<T>> {
    static TypeData Create() {
        return {
            .name = "vector",
            ._namespace = "std",
            .inners = {
                ReflectID<T>()
            },
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<std::vector<T>> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::vector<";
        type.GetInner(0).Value().get().Print(stream);
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

template <typename T_>
struct ReflCpp::ReflectData<std::unique_ptr<T_>> {
    static TypeData Create() {
        return {
            .name = "Unique Pointer",
            ._namespace = "std",
            .inners = {
                ReflectID<T_>(),
            },
            .flags = TypeFlags::IsPointer
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<std::unique_ptr<T_>> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::unique_ptr<";
        type.GetInner(0).Value().get().Print(stream);
        stream << ">";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<std::shared_ptr<T_>> {
    static TypeData Create() {
        return {
            .name = "Shared Pointer",
            ._namespace = "std",
            .inners = {
                ReflectID<T_>(),
            },
            .flags = TypeFlags::IsPointer
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<std::shared_ptr<T_>> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::shared_ptr<";
        type.GetInner(0).Value().get().Print(stream);
        stream << ">";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<std::weak_ptr<T_>> {
    static TypeData Create() {
        return {
            .name = "Weak Pointer",
            ._namespace = "std",
            .inners = {
                ReflectID<T_>(),
            },
            .flags = TypeFlags::IsPointer
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<std::weak_ptr<T_>> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::weak_ptr<";
        type.GetInner(0).Value().get().Print(stream);
        stream << ">";
    }
};