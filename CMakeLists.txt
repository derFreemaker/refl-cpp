cmake_minimum_required(VERSION 3.19)
set(CMAKE_CXX_STANDARD 23)

project(refl-cpp)

include_directories(include)

include(cmake/CPM.cmake)

CPMAddPackage(
        NAME fmt
        GITHUB_REPOSITORY fmtlib/fmt
        GIT_TAG 11.1.4
        OPTIONS
            "FMT_INSTALL OFF"
)

add_library(refl-cpp INTERFACE include/refl-cpp/refl-cpp.hpp
        include/refl-cpp/common/list_seperator.hpp
        include/refl-cpp/common/flags.hpp
        include/refl-cpp/common/formatted_error.hpp
        include/refl-cpp/common/stack_tracing_error.hpp
        include/refl-cpp/common/result.hpp
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

        include/refl-cpp/meta/meta.hpp
        include/refl-cpp/meta/builtin.hpp
        include/refl-cpp/meta/common.hpp
        include/refl-cpp/meta/std.hpp

        include/refl-cpp/impl/impl.hpp
        include/refl-cpp/impl/variant.hpp
)
target_link_libraries(refl-cpp INTERFACE
    fmt::fmt
)

# small test project
add_subdirectory(test-project)

if (ENABLE_REFLCPP_GENERATOR)
    add_subdirectory(generator)
endif ()

if (ENABLE_REFLCPP_TESTS)
    add_subdirectory(tests)
endif ()