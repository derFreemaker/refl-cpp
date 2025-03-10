#pragma once

namespace ReflCpp {

struct Type;
using ReflectPrintFunc = std::function<void(std::ostream& stream, const Type& type)>;

template <typename T>
struct ReflectPrinter;

template <typename T>
concept has_reflect_printer = requires(std::ostream& stream, const Type& type) {
    { ReflectPrinter<T>::Print(stream, type) } -> std::same_as<void>;
};

}