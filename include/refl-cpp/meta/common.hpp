#pragma once

#include "refl-cpp/declare_reflect.hpp"

// Array

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DECL(T[])

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DEF(T[])
    {
        .name = "Array",
        .inners = ReflCpp::ReflectID<T>().value(),
        .flags = ReflCpp::TypeFlags::IsArray,
    }
REFLCPP_REFLECT_DATA_DEF_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T[])
    type.GetInner(0).value().Print(out);
    out << "[]";
REFLCPP_REFLECT_PRINTER_END()

// Pointer

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DECL(T*)

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DEF(T*)
    {
        .name = "Pointer",
        .inners = {
            ReflCpp::ReflectID<T>().value()
        },
        .flags = ReflCpp::TypeFlags::IsPointer,
    }

REFLCPP_REFLECT_DATA_DEF_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T*)
    type.GetInner(0).value().Print(out);
    out << "*";
REFLCPP_REFLECT_PRINTER_END()

// Const Pointer

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DECL(T* const)

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DEF(T* const)
    {
        .name = "Const Pointer",
        .inners = {
            ReflCpp::ReflectID<T>().value()
        },
        .flags = ReflCpp::TypeFlags::IsPointer | ReflCpp::TypeFlags::IsConst,
    }
REFLCPP_REFLECT_DATA_DEF_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T* const)
    type.GetInner(0).value().Print(out);
    out << "* const";
REFLCPP_REFLECT_PRINTER_END()

// LValue Reference

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DECL(T&)

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DEF(T&)
    {
        .name = "LValue Reference",
        .inners = {
            ReflCpp::ReflectID<T>().value()
        },
        .flags = ReflCpp::TypeFlags::IsLValueReference,
    }
REFLCPP_REFLECT_DATA_DEF_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T&)
    type.GetInner(0).value().Print(out);
    out << "&";
REFLCPP_REFLECT_PRINTER_END()

// RValue Reference

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DECL(T&&)

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DEF(T&&)
    {
        .name = "RValue Reference",
        .inners = {
            ReflCpp::ReflectID<T>().value()
        },
        .flags = ReflCpp::TypeFlags::IsRValueReference,
    }
REFLCPP_REFLECT_DATA_DEF_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T&&)
    type.GetInner(0).value().Print(out);
    out << "&&";
REFLCPP_REFLECT_PRINTER_END()

// Const

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DECL(const T)

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DEF(const T)
    {
        .name = "Constant",
        .inners = {
            ReflCpp::ReflectID<T>().value()
        },
        .flags = ReflCpp::TypeFlags::IsConst,
    }
REFLCPP_REFLECT_DATA_DEF_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(const T)
    out << "const ";
    type.GetInner(0).value().Print(out);
REFLCPP_REFLECT_PRINTER_END()

// Volatile

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DECL(volatile T)

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA_DEF(volatile T)
    {
        .name = "Volatile",
        .inners = {
            ReflCpp::ReflectID<T>().value()
        },
        .flags = ReflCpp::TypeFlags::IsVolatile,
    }
REFLCPP_REFLECT_DATA_DEF_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(volatile T)
    out << "volatile ";
    type.GetInner(0).value().Print(out);
REFLCPP_REFLECT_PRINTER_END()
