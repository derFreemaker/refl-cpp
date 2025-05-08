cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 23)

project(refl-cpp_tests)

CPMAddPackage(
        NAME Catch2
        VERSION 3.8.1
        GITHUB_REPOSITORY catchorg/Catch2
)

add_executable(refl-cpp_tests
        helper/variant_helper.hpp

        variant.cpp
        function_wrapper.cpp
        method.cpp
        no_copy_or_move_struct.hpp
)
target_link_libraries(refl-cpp_tests PRIVATE
        Catch2::Catch2WithMain
        refl-cpp
)
