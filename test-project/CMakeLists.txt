cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 20)

project(refl-cpp-test)

# load refl-cpp library
#add_subdirectory("../")

add_executable(refl-cpp-test
        main.cpp
        refl-cpp.generated.hpp
)
target_link_libraries(refl-cpp-test
        refl-cpp
)
