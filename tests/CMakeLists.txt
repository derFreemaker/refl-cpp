cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 23)

project(refl-cpp_tests)

CPMAddPackage(
        NAME GoogleTest
        VERSION 1.16.0
        GITHUB_REPOSITORY google/googletest
        OPTIONS
        "INSTALL_GTEST OFF"
        "BUILD_GMOCK ON"
)

add_executable(refl-cpp_tests
        helper/variant_helper.hpp

        variant.cpp
        function_wrapper.cpp
        method.cpp
        no_copy_or_move_struct.hpp
)
target_link_libraries(refl-cpp_tests PRIVATE
        gtest
        gtest_main
        gmock
        refl-cpp
)
