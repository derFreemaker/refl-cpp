#pragma once

#include "refl-cpp/declare_reflect.hpp"

template <typename T_>
struct ReflCpp::ReflectData<T_[]> {
    static Result<TypeData> Create() {
        return {
            RESULT_OK(), {
                .name = "Array",
                .inners = TRY(ReflectID<T_>()),
                .flags = TypeFlags::IsArray,
            }
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_[]> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().Print(stream);
        stream << "[]";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_*> {
    static Result<TypeData> Create() {
        return {
            RESULT_OK(), {
                .name = "Pointer",
                .inners = {
                    TRY(ReflectID<T_>())
                },
                .flags = TypeFlags::IsPointer,
            }
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_*> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().Print(stream);
        stream << "*";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_* const> {
    static Result<TypeData> Create() {
        return {
            RESULT_OK(), {
                .name = "Constant Pointer",
                .inners = {
                    TRY(ReflectID<T_>())
                },
                .flags = TypeFlags::IsPointer | TypeFlags::IsConst,
            }
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_* const> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().Print(stream);
        stream << "* const";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_&> {
    static Result<TypeData> Create() {
        return {
            RESULT_OK(), {
                .name = "LValue Reference",
                .inners = {
                    TRY(ReflectID<T_>())
                },
                .flags = TypeFlags::IsLValueReference,
            }
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_&> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().Print(stream);
        stream << "&";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<T_&&> {
    static Result<TypeData> Create() {
        return {
            RESULT_OK(), {
                .name = "RValue Reference",
                .inners = {
                    TRY(ReflectID<T_>())
                },
                .flags = TypeFlags::IsRValueReference,
            }
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<T_&&> {
    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner(0).Value().Print(stream);
        stream << "&&";
    }
};

template <typename T_>
struct ReflCpp::ReflectData<const T_> {
    static Result<TypeData> Create() {
        return {
            RESULT_OK(), {
                .name = "Constant",
                .inners = {
                    TRY(ReflectID<T_>())
                },
                .flags = TypeFlags::IsConst,
            }
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<const T_> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "const ";
        type.GetInner(0).Value().Print(stream);
    }
};

template <typename T_>
struct ReflCpp::ReflectData<volatile T_> {
    static Result<TypeData> Create() {
        return {
            RESULT_OK(), {
                .name = "Volatile",
                .inners = {
                    TRY(ReflectID<T_>())
                },
                .flags = TypeFlags::IsVolatile,
            }
        };
    }
};

template <typename T_>
struct ReflCpp::ReflectPrinter<volatile T_> {
    static void Print(std::ostream& stream, const Type& type) {
        stream << "volatile ";
        type.GetInner(0).Value().Print(stream);
    }
};
