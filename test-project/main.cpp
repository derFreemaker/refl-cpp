#include <optional>

#include "refl-cpp/refl-cpp.hpp"

#include "test.hpp"

int main() {
    kpop::Test test;

    (void)ReflCpp::Reflect<kpop::Test>()
          .GetMethod("foo")->Invoke(
              test, {
                  "test",
                  "test"
              }
          );
}
