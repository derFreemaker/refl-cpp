#include <benchmark/benchmark.h>

#include "refl-cpp/common/result.hpp"

static ReflCpp::Result<void> ReturnNoFail() {
    return {};
}

static ReflCpp::Result<void> ReturnFail() {
    return { RESULT_ERROR(), "failed" };
}

static void BM_ReturnSimpleNoFail(benchmark::State& state) {
    for (auto _ : state) {
        auto result = ReturnNoFail();
    }
}
BENCHMARK(BM_ReturnSimpleNoFail);

static void BM_ReturnSimpleFail(benchmark::State& state) {
    for (auto _ : state) {
        auto result = ReturnFail();
    }
}
BENCHMARK(BM_ReturnSimpleFail);
