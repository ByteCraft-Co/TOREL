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

add_executable(torelc
    compiler/driver/main.cpp
)

target_include_directories(torelc
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/compiler
        ${LLVM_INCLUDE_DIRS}
)

target_compile_definitions(torelc
    PRIVATE
        ${LLVM_DEFINITIONS}
)

if(MSVC)
    target_compile_options(torelc PRIVATE /W4 /permissive-)
else()
    target_compile_options(torelc PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(TOREL_BUILD_TESTS)
    include(CTest)

    add_test(
        NAME compiler.starts
        COMMAND torelc
    )
endif()
