cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

project(refl-cpp_tests)

include(../cmake/CPM.cmake)

CPMAddPackage(
        NAME GoogleTest
        VERSION 1.16.0
        GITHUB_REPOSITORY google/googletest
        OPTIONS
        "INSTALL_GTEST OFF"
        "BUILD_GMOCK ON"
)

add_executable(refl-cpp_tests
        reflect.cpp
        fields.cpp
        function_wrapper.cpp
        variant.cpp
)
target_link_libraries(refl-cpp_tests PRIVATE
        gtest
        gtest_main
        gmock
        refl-cpp
)