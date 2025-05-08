cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 26)

project(refl-cpp-test)

# load refl-cpp library

# will have to be created automatically
#add_library(refl-cpp-test_reflection STATIC
#        refl-cpp.generated/kpop/test.generated.hpp 
#        refl-cpp.generated/kpop/test.generated.cpp
#)
#target_link_libraries(refl-cpp-test_reflection PUBLIC
#    refl-cpp
#)
#target_include_directories(refl-cpp-test_reflection PUBLIC
#        ${CMAKE_SOURCE_DIR}/example
#)

add_executable(refl-cpp-test main.cpp
        test.hpp
)
target_link_libraries(refl-cpp-test
        refl-cpp
#        refl-cpp-test_reflection
)
target_include_directories(refl-cpp-test PUBLIC
        ${CMAKE_SOURCE_DIR}/example
)
