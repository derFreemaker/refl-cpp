cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 23)

project(refl-cpp)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME Ccache.cmake
        GITHUB_REPOSITORY TheLartians/Ccache.cmake
        VERSION 1.2.5
        GIT_SHALLOW ON
        PATCHES
        patches/ccache.cmake.patch
)

CPMAddPackage(
        NAME res-cpp
        GITHUB_REPOSITORY derFreemaker/res-cpp
        VERSION 0.1.0
        GIT_SHALLOW ON
)

add_library(refl-cpp INTERFACE include/refl-cpp/refl-cpp.hpp
        include/refl-cpp/common/list_seperator.hpp
        include/refl-cpp/common/flags.hpp
        include/refl-cpp/common/type_traits.hpp

        include/refl-cpp/type_id.hpp
        include/refl-cpp/type.hpp
        include/refl-cpp/type_data.hpp
        include/refl-cpp/type_instance.hpp
        include/refl-cpp/argument.hpp
        include/refl-cpp/database.hpp
        include/refl-cpp/reflect_printer.hpp
        include/refl-cpp/declare_reflect.hpp
        include/refl-cpp/reflect.hpp
        include/refl-cpp/variant.hpp
        include/refl-cpp/function_traits.hpp
        include/refl-cpp/method_wrapper.hpp
        include/refl-cpp/method_data.hpp
        include/refl-cpp/method.hpp
        include/refl-cpp/field_traits.hpp
        include/refl-cpp/field_data.hpp
        include/refl-cpp/field.hpp
        include/refl-cpp/field_wrapper.hpp
        include/refl-cpp/type_flags.hpp
        include/refl-cpp/function_wrapper.hpp
        include/refl-cpp/method_func.hpp
        include/refl-cpp/method_func_data.hpp

        include/refl-cpp/meta/meta.hpp
        include/refl-cpp/meta/builtin.hpp
        include/refl-cpp/meta/common.hpp
        include/refl-cpp/meta/std.hpp

        include/refl-cpp/impl/impl.hpp
        include/refl-cpp/impl/variant.hpp
        include/refl-cpp/impl/variant_matcher.hpp
        include/refl-cpp/impl/variant_wrapper.hpp
        include/refl-cpp/impl/variant_matcher/value_variant_matcher.hpp
        include/refl-cpp/impl/variant_matcher/const_value_variant_matcher.hpp
        include/refl-cpp/impl/variant_matcher/lvalue_ref_variant_matcher.hpp
        include/refl-cpp/impl/variant_matcher/const_lvalue_ref_variant_matcher.hpp
        include/refl-cpp/impl/variant_matcher/rvalue_ref_variant_matcher.hpp
        include/refl-cpp/impl/method_func_data.hpp
)
target_include_directories(refl-cpp INTERFACE include)
target_link_libraries(refl-cpp INTERFACE
        res-cpp
)
target_precompile_headers(refl-cpp INTERFACE
        include/refl-cpp/refl-cpp.hpp
)

if (ENABLE_REFLCPP_EXAMPLE_PROJECT)
    add_subdirectory(example)
endif ()

if (ENABLE_REFLCPP_GENERATOR)
    add_subdirectory(generator)
endif ()

if (ENABLE_REFLCPP_TESTS)
    add_subdirectory(tests)
endif ()
