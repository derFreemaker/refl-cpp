#include <benchmark/benchmark.h>

#define REFLCPP_ENABLE_STACK_TRACING_ERROR 0

#include "refl-cpp/common/result.hpp"

static ReflCpp::Result<void> ReturnSimpleNoFail() {
    return {};
}

static void BM_ReturnSimpleNoFail(benchmark::State& state) {
    for (auto _ : state) {
        auto result = ReturnSimpleNoFail();
    }
}
BENCHMARK(BM_ReturnSimpleNoFail);

static ReflCpp::Result<void> ReturnSimpleFail() {
    return { RESULT_ERROR(), "failed" };
}

static void BM_ReturnSimpleFail(benchmark::State& state) {
    for (auto _ : state) {
        auto result = ReturnSimpleFail();
    }
}
BENCHMARK(BM_ReturnSimpleFail);

static ReflCpp::Result<void> ReturnNoFail(const int depth) {
    if (depth == 0) {
        return {};
    }
    return ReturnNoFail(depth - 1);
}

static void BM_ReturnNoFail(benchmark::State& state) {
    for (auto _ : state) {
        auto result = ReturnNoFail(100000);
    }
}
BENCHMARK(BM_ReturnNoFail);

static ReflCpp::Result<void> ReturnFail(const int depth) {
    if (depth == 0) {
        return { RESULT_ERROR(), "failed" };
    }
    return ReturnFail(depth - 1);
}

static void BM_ReturnFail(benchmark::State& state) {
    for (auto _ : state) {
        auto result = ReturnFail(100000);
    }
}
BENCHMARK(BM_ReturnFail);
