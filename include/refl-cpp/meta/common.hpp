#pragma once

#include "refl-cpp/declare_reflect.hpp"

// Array

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA(T[]){
    .name = "Array",
    .inners = ReflectID<T>().value(),
    .flags = TypeFlags::IsArray,
}
REFLCPP_REFLECT_DATA_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T[])
    type.GetInner(0).value().Print(out);
    out << "[]";
REFLCPP_REFLECT_PRINTER_END()

// Pointer

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA(T*){
    .name = "Pointer",
    .inners = {
        ReflectID<T>().value()
    },
    .flags = TypeFlags::IsPointer,
}
REFLCPP_REFLECT_DATA_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T*)
    type.GetInner(0).value().Print(out);
    out << "*";
REFLCPP_REFLECT_PRINTER_END()

// Const Pointer

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA(T* const){
    .name = "Const Pointer",
    .inners = {
        ReflectID<T>().value()
    },
    .flags = TypeFlags::IsPointer | TypeFlags::IsConst,
}
REFLCPP_REFLECT_DATA_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T* const)
    type.GetInner(0).value().Print(out);
    out << "* const";
REFLCPP_REFLECT_PRINTER_END()

// LValue Reference

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA(T&){
    .name = "LValue Reference",
    .inners = {
        ReflectID<T>().value()
    },
    .flags = TypeFlags::IsLValueReference,
}
REFLCPP_REFLECT_DATA_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T&)
    type.GetInner(0).value().Print(out);
    out << "&";
REFLCPP_REFLECT_PRINTER_END()

// RValue Reference

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA(T&&){
    .name = "RValue Reference",
    .inners = {
        ReflectID<T>().value()
    },
    .flags = TypeFlags::IsRValueReference,
}
REFLCPP_REFLECT_DATA_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(T&&)
    type.GetInner(0).value().Print(out);
    out << "&&";
REFLCPP_REFLECT_PRINTER_END()

// Const

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA(const T){
    .name = "Constant",
    .inners = {
        ReflectID<T>().value()
    },
    .flags = TypeFlags::IsConst,
}
REFLCPP_REFLECT_DATA_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(const T)
    out << "const ";
    type.GetInner(0).value().Print(out);
REFLCPP_REFLECT_PRINTER_END()

// Volatile

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_DATA(volatile T){
    .name = "Volatile",
    .inners = {
        ReflectID<T>().value()
    },
    .flags = TypeFlags::IsVolatile,
}
REFLCPP_REFLECT_DATA_END()

REFLCPP_REFLECT_TEMPLATE(typename T)
REFLCPP_REFLECT_PRINTER(volatile T)
    out << "volatile ";
    type.GetInner(0).value().Print(out);
REFLCPP_REFLECT_PRINTER_END()
