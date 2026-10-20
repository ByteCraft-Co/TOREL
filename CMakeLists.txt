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
# Diagnostics
# -----------------------------------------------------------------------------

add_library(torel_diagnostics
    compiler/diagnostics/src/DiagnosticEngine.cpp
)

target_include_directories(torel_diagnostics
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/compiler/diagnostics/include
)

target_compile_features(torel_diagnostics
    PUBLIC
        cxx_std_23
)

target_link_libraries(torel_diagnostics
    PUBLIC
        torel_source
)

torel_enable_warnings(torel_diagnostics)

# -----------------------------------------------------------------------------
# Lexer
# -----------------------------------------------------------------------------

add_library(torel_lexer
    compiler/lexer/src/Lexer.cpp
)

target_include_directories(torel_lexer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/compiler/lexer/include
)

target_compile_features(torel_lexer
    PUBLIC
        cxx_std_23
)

target_link_libraries(torel_lexer
    PUBLIC
        torel_source
        torel_diagnostics
)

torel_enable_warnings(torel_lexer)
# -----------------------------------------------------------------------------
# Parser
# -----------------------------------------------------------------------------

add_library(torel_parser
    compiler/parser/src/TokenCursor.cpp
    compiler/parser/src/Parser.cpp
)

target_include_directories(torel_parser
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/compiler/parser/include
)

target_compile_features(torel_parser
    PUBLIC
        cxx_std_23
)

target_link_libraries(torel_parser
    PUBLIC
        torel_lexer
        torel_diagnostics
)

torel_enable_warnings(torel_parser)
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
        torel_diagnostics
        torel_lexer
        torel_parser
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

    add_executable(torel_diagnostics_tests
        tests/diagnostics/DiagnosticTests.cpp
    )

    target_link_libraries(torel_diagnostics_tests
        PRIVATE
            torel_diagnostics
    )

    torel_enable_warnings(torel_diagnostics_tests)

    add_test(
        NAME diagnostics.foundation
        COMMAND torel_diagnostics_tests
    )
    add_executable(torel_lexer_tests
        tests/lexer/LexerTests.cpp
    )

    target_link_libraries(torel_lexer_tests
        PRIVATE
            torel_lexer
    )

    torel_enable_warnings(torel_lexer_tests)

    add_test(
        NAME lexer.foundation
        COMMAND torel_lexer_tests
    )
    add_executable(torel_parser_tests
        tests/parser/ParserTests.cpp
    )

    target_link_libraries(torel_parser_tests
        PRIVATE
            torel_parser
    )

    torel_enable_warnings(torel_parser_tests)

    add_test(
        NAME parser.foundation
        COMMAND torel_parser_tests
    )
endif()