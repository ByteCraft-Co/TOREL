cmake_minimum_required(VERSION 3.25)

project(
    TOREL
    VERSION 0.0.1
    DESCRIPTION "Typed Ownership and Relational Execution Language"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(TOREL_BUILD_TESTS "Build TOREL tests" ON)

find_package(LLVM REQUIRED CONFIG)

message(STATUS "TOREL version: ${PROJECT_VERSION}")
message(STATUS "LLVM version: ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM directory: ${LLVM_DIR}")

function(torel_enable_warnings target)
    if(MSVC)
        target_compile_options(
            ${target}
            PRIVATE
                /W4
                /permissive-
        )
    else()
        target_compile_options(
            ${target}
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()
endfunction()

# -----------------------------------------------------------------------------
# Source infrastructure
# -----------------------------------------------------------------------------

add_library(torel_source
    compiler/source/src/SourceFile.cpp
    compiler/source/src/SourceManager.cpp
)

target_include_directories(torel_source
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/compiler/source/include
)

target_compile_features(torel_source
    PUBLIC
        cxx_std_23
)

torel_enable_warnings(torel_source)

# -----------------------------------------------------------------------------
# Stage 0 compiler
# -----------------------------------------------------------------------------

add_executable(torelc
    compiler/driver/main.cpp
)

target_include_directories(torelc
    PRIVATE
        ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(torelc
    PRIVATE
        ${LLVM_DEFINITIONS}
)

target_link_libraries(torelc
    PRIVATE
        torel_source
)

torel_enable_warnings(torelc)

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------

if(TOREL_BUILD_TESTS)
    include(CTest)

    add_test(
        NAME compiler.starts
        COMMAND torelc
    )

    add_executable(torel_source_tests
        tests/source/SourceInfrastructureTests.cpp
    )

    target_link_libraries(torel_source_tests
        PRIVATE
            torel_source
    )

    torel_enable_warnings(torel_source_tests)

    add_test(
        NAME source.infrastructure
        COMMAND torel_source_tests
    )
endif()