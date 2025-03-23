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
        common/result.cpp
        
        variant.cpp
        helper/result_helper.hpp
        helper/variant_helper.hpp
        function_wrapper.cpp
        fields.cpp
)
target_link_libraries(refl-cpp_tests PRIVATE
        gtest
        gtest_main
        gmock
        refl-cpp
)