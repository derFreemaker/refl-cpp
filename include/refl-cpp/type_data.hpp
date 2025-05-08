#pragma once

#include <vector>
#include <optional>

#include "refl-cpp/reflect_printer.hpp"
#include "refl-cpp/field.hpp"
#include "refl-cpp/method.hpp"
#include "refl-cpp/type_id.hpp"
#include "refl-cpp/type_flags.hpp"

namespace ReflCpp {
struct TypeData {
    const char* name = "$NONE$";
    std::optional<const char*> _namespace = std::nullopt;

    std::vector<TypeID> bases;
    std::vector<TypeID> inners;

    TypeFlags flags;

    std::vector<Field> fields;
    std::vector<Method> methods;
};

struct TypeOptions {
    ReflectPrintFunc printFunc = nullptr;
};

template <typename T>
struct ReflectData;
}

#include "refl-cpp/reflect_printer.hpp"

#define REFLCPP_REFLECT_TEMPLATE(...) \
        template <__VA_ARGS__>

#define REFLCPP_REFLECT_DATA_DECL(...) \
    struct ReflCpp::ReflectData<__VA_ARGS__> { \
    private: \
        static ReflCpp::TypeData CreateImpl(); \
    public: \
        static const TypeData& Create() { \
            static TypeData data = CreateImpl(); \
            return data; \
        } \
    };

#define REFLCPP_REFLECT_DATA_DEF(...) \
    inline ReflCpp::TypeData ReflCpp::ReflectData<__VA_ARGS__>::CreateImpl() { \
        return ReflCpp::TypeData

#define REFLCPP_REFLECT_DATA_DEF_END() \
        ; \
    }

#define REFLCPP_REFLECT_PRINTER(...) \
    struct ReflCpp::ReflectPrinter<__VA_ARGS__> { \
        static void Print(std::ostream& out, const Type& type) {

#define REFLCPP_REFLECT_PRINTER_END() \
        } \
    };
