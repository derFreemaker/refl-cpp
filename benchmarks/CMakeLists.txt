cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 23)

project(refl-cpp_benchmarks)

CPMAddPackage(
        NAME benchmark
        GITHUB_REPOSITORY google/benchmark
        VERSION 1.9.2
        OPTIONS
        "BENCHMARK_ENABLE_TESTING OFF"
        "BENCHMARK_ENABLE_INSTALL OFF"
)

add_executable(refl-cpp_benchmarks
    result.cpp
)
target_link_libraries(refl-cpp_benchmarks PRIVATE
        refl-cpp
        
        benchmark::benchmark
        benchmark::benchmark_main
)