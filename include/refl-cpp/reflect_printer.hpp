#pragma once

#include <functional>

namespace ReflCpp {
struct Type;
using ReflectPrintFunc = std::function<void(std::ostream& stream, const Type& type)>;

template <typename T>
struct ReflectPrinter;

namespace detail {
template <typename T>
concept HasReflectPrinter = requires(std::ostream& stream, const Type& type) {
    { ReflectPrinter<T>::Print(stream, type) } -> std::same_as<void>;
};
}
}
