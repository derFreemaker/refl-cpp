cmake_minimum_required(VERSION 3.30)
set(CMAKE_CXX_STANDARD 20)

project(refl-cpp_generator)

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

add_executable(refl-cpp_generator src/main.cpp
        src/generate_reflect_data.hpp
        src/generate_reflect_data.cpp
        src/reflect_data.hpp)
target_link_libraries(refl-cpp_generator PUBLIC
        refl-cpp_llvm
)
