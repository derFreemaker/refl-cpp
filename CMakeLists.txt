cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

project(refl-cpp)

include_directories("src")
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif ()

# --- LLVM --- #

CPMAddPackage(
        NAME llvm-project
        VERSION 19.1.7
        GITHUB_REPOSITORY llvm/llvm-project
        GIT_TAG llvmorg-19.1.7
        GIT_PROGRESS ON
        GIT_SHALLOW ON
        DOWNLOAD_ONLY YES
)

set(LLVM_TARGETS_TO_BUILD "X86" CACHE STRING "")
set(LLVM_ENABLE_PROJECTS "llvm;clang;clang-tools-extra" CACHE STRING "")

add_subdirectory(
        ${llvm-project_SOURCE_DIR}/llvm
        ${CMAKE_BINARY_DIR}/_deps/llvm-build
)

add_library(refl-cpp_llvm INTERFACE)
target_link_libraries(refl-cpp_llvm INTERFACE
        clangTooling
        clangASTMatchers
        clangBasic
        clangAST
        clangFrontend
        LLVMSupport
)
target_include_directories(refl-cpp_llvm INTERFACE
        ${llvm-project_SOURCE_DIR}/clang/include
        ${llvm-project_SOURCE_DIR}/llvm/include

        ${CMAKE_BINARY_DIR}/_deps/llvm-build/include
        ${CMAKE_BINARY_DIR}/_deps/llvm-build/tools/clang/include
)

# --- LLVM --- #

add_executable(refl-cpp
        src/main.cpp
)
target_link_libraries(refl-cpp PUBLIC
        refl-cpp_llvm
)
