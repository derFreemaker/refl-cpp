#pragma once

#include <optional>
#include <vector>

#include "refl-cpp/declare_reflect.hpp"
#include "refl-cpp/reflect_printer.hpp"

template <>
struct ReflCpp::ReflectData<std::string> {
    static TypeData Create() {
        return TypeData {
            .name = "string",
            ._namespace = "std"
        };
    }
};

template <typename T>
struct ReflCpp::ReflectData<std::reference_wrapper<T>> {
    static TypeData Create() {
        return TypeData {
            .name = "reference_wrapper",
            ._namespace = "std",
            .inners = {
                ReflectID<T>(),
            },
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<std::reference_wrapper<T>> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::reference_wrapper<";
        type.GetInner(0).value().Print(stream);
        stream << ">";
    }
};

template <typename T>
struct ReflCpp::ReflectData<std::optional<T>> {
    static TypeData Create() {
        return TypeData {
            .name = "optional",
            ._namespace = "std",
            .inners = {
                ReflectID<T>(),
            },

            //TODO: take care of lvalue and rvalue references

            // .methods = {
            //     MethodData<const T& (std::optional<T>::*)() const&> {
            //         .ptr = &std::optional<T>::value,
            //         .name = "value",
            //     },
            //     MethodData<T& (std::optional<T>::*)() &> {
            //         .ptr = &std::optional<T>::value,
            //         .name = "value",
            //     },
            //     MethodData<const T&& (std::optional<T>::*)() const&&> {
            //         .ptr = &std::optional<T>::value,
            //         .name = "value",
            //     },
            //     MethodData<T&& (std::optional<T>::*)() &&> {
            //         .ptr = &std::optional<T>::value,
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
        type.GetInner(0).value().Print(stream);
        stream << ">";
    }
};

template <typename T>
struct ReflCpp::ReflectData<std::vector<T>> {
    static TypeData Create() {
        return TypeData {
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
        type.GetInner(0).value().Print(stream);
        stream << ">";
    }
};

template <>
struct ReflCpp::ReflectData<std::nullptr_t> {
    static TypeData Create() {
        return TypeData {
            .name = "Null Pointer",
            ._namespace = "std",
        };
    }
};

template <>
struct ReflCpp::ReflectPrinter<std::nullptr_t> {
    static void Print(std::ostream& stream, const Type& type) noexcept {
        stream << "nullptr";
    }
};

template <typename T>
struct ReflCpp::ReflectData<std::unique_ptr<T>> {
    static TypeData Create() noexcept {
        return TypeData {
            .name = "Unique Pointer",
            ._namespace = "std",
            .inners = {
                ReflectID<T>(),
            },
            .flags = TypeFlags::IsPointer
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<std::unique_ptr<T>> {
    static void Print(std::ostream& stream, const Type& type) noexcept {
        stream << "std::unique_ptr<";
        type.GetInner(0).value().Print(stream);
        stream << ">";
    }
};

template <typename T>
struct ReflCpp::ReflectData<std::shared_ptr<T>> {
    static TypeData Create() noexcept {
        return TypeData {
            .name = "Shared Pointer",
            ._namespace = "std",
            .inners = {
                ReflectID<T>(),
            },
            .flags = TypeFlags::IsPointer
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<std::shared_ptr<T>> {
    static void Print(std::ostream& stream, const Type& type) noexcept {
        stream << "std::shared_ptr<";
        type.GetInner(0).value().Print(stream);
        stream << ">";
    }
};

template <typename T>
struct ReflCpp::ReflectData<std::weak_ptr<T>> {
    static TypeData Create() noexcept {
        return {
            .name = "Weak Pointer",
            ._namespace = "std",
            .inners = {
                ReflectID<T>(),
            },
            .flags = TypeFlags::IsPointer
        };
    }
};

template <typename T>
struct ReflCpp::ReflectPrinter<std::weak_ptr<T>> {
    static void Print(std::ostream& stream, const Type& type) noexcept {
        stream << "std::weak_ptr<";
        type.GetInner(0).value().Print(stream);
        stream << ">";
    }
};
