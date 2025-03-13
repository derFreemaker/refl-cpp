cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 23)

project(refl-cpp-test)

# load refl-cpp library
#add_subdirectory("../")

add_executable(refl-cpp-test main.cpp

        test.hpp
        refl-cpp.generated/kpop/test.generated.hpp
        refl-cpp.generated/kpop/test.generated.cpp
)
target_link_libraries(refl-cpp-test
        refl-cpp
)
target_include_directories(refl-cpp-test PUBLIC
        ${CMAKE_SOURCE_DIR}/test-project
)
