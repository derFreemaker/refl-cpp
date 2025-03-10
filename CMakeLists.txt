cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

project(refl-cpp)
include_directories(include)

include_directories("src")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif ()

CPMAddPackage(
        NAME Ccache.cmake
        GITHUB_REPOSITORY TheLartians/Ccache.cmake
        VERSION 1.2.5
        GIT_PROGRESS ON
        GIT_SHALLOW ON
        PATCHES
            patches/ccache.cmake.patch
)

# --- LLVM --- #
set(LLVM_ENABLE_PROJECTS "clang;clang-tools-extra" CACHE STRING "")
set(LLVM_ENABLE_RTTI "ON" CACHE STRING "")

CPMAddPackage(
        NAME llvm-project
        VERSION 19.1.7
        GITHUB_REPOSITORY llvm/llvm-project
        GIT_TAG llvmorg-19.1.7
        GIT_PROGRESS ON
        GIT_SHALLOW ON
        SOURCE_SUBDIR "llvm"
)

add_library(refl-cpp_llvm INTERFACE)
target_link_libraries(refl-cpp_llvm INTERFACE
        clangTooling
        clangFrontend
        clangSerialization
        clangDriver
        clangParse
        clangSema
        clangAnalysis
        clangEdit
        clangAST
        clangLex
        clangBasic
        LLVMCore
        LLVMSupport
)
target_include_directories(refl-cpp_llvm INTERFACE
        ${llvm-project_SOURCE_DIR}/llvm/include
        ${CMAKE_BINARY_DIR}/_deps/llvm-project-build/include

        ${llvm-project_SOURCE_DIR}/clang/include
        ${CMAKE_BINARY_DIR}/_deps/llvm-project-build/tools/clang/include
)
# --- LLVM --- #

add_library(refl-cpp STATIC include/refl-cpp/refl-cpp.hpp
        include/refl-cpp/type_id.hpp
        include/refl-cpp/type.hpp
        include/refl-cpp/reflect_data.hpp
        include/refl-cpp/reflect_data_instance.hpp
        include/refl-cpp/function.hpp
        include/refl-cpp/argument.hpp
        include/refl-cpp/database.hpp
        
        src/refl-cpp/argument.cpp
        src/refl-cpp/type.cpp
)

add_executable(refl-cpp_generator
        src/generator/main.cpp
)
target_link_libraries(refl-cpp_generator PUBLIC
        refl-cpp_llvm
)

add_subdirectory(test-project)
