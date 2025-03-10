#pragma once

#include "refl-cpp/type.hpp"
#include "refl-cpp/reflect_data_instance.hpp"
#include "refl-cpp/database.hpp"

namespace ReflCpp {
template <typename T>
const Type& Reflect() {
    return ReflectDataInstance<T>::Instance();
}

[[nodiscard]]
inline const Type& Reflect(const TypeID id) {
    return ReflectionDatabase::Instance().GetType(id);
}

// universal reflect data

//TODO: find better way than this:

template <typename T>
struct ReflectData<T[]> {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Array",
            .inner = Reflect<T>(),
            .is_array = true,
        };
    }

    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner().Print(stream);
        stream << "[]";
    }
};

template <typename T>
struct ReflectData<T*> {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Pointer",
            .inner = Reflect<T>(),
            .is_pointer = true,
        };
    }

    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner().Print(stream);
        stream << "*";
    }
};

template <typename T>
struct ReflectData<T&> {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Reference",
            .inner = Reflect<T>(),
            .is_reference = true,
        };
    }

    static void Print(std::ostream& stream, const Type& type) {
        type.GetInner().Print(stream);
        stream << "&";
    }
};

template <typename T>
struct ReflectData<const T> {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Constant",
            .inner = Reflect<T>(),
            .is_const = true,
        };
    }

    static void Print(std::ostream& stream, const Type& type) {
        stream << "const ";
        type.GetInner().Print(stream);
    }
};

template <typename T>
struct ReflectData<volatile T> {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "Volatile",
            .inner = Reflect<T>(),
            .is_volatile = true,
        };
    }

    static void Print(std::ostream& stream, const Type& type) {
        stream << "volatile ";
        type.GetInner().Print(stream);
    }
};

// std reflect data

template <typename T>
struct ReflectData<std::optional<T>> {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "std::optional",
            .inner = Reflect<T>(),
        };
    }

    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::optional<";
        type.GetInner().Print(stream);
        stream << ">";
    }
};

template <typename T>
struct ReflectData<std::vector<T>> {
    static ReflectTypeData Create() {
        return ReflectTypeData{
            .name = "std::vector",
            .inner = Reflect<T>(),
        };
    }

    static void Print(std::ostream& stream, const Type& type) {
        stream << "std::vector<";
        type.GetInner().Print(stream);
        stream << ">";
    }
};
}

#define REFLECT() __attribute__((annotate("reflect")))
