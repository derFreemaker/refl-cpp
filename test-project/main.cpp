#include <stdio.h>

#include <optional>

#define REFLECT __attribute__((annotate("reflect")))

namespace kpop {
class REFLECT Test {
    int *foo;
};
}

struct REFLECT Test2 {
    std::optional<bool> foo;
};

enum REFLECT EnumTest {
    NONE = 0,
    TEST = 0,
};

int main() {
    printf("Hello World");
}
