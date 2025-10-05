cmake_minimum_required(VERSION 3.11)
project(cpplogger)

include(cmake/generate_header.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CPPLOGGER_NO_EXCEPTIONS ON)

option(CPPLOGGER_SHARED_LIB OFF)
if(CPPLOGGER_SHARED_LIB)
    set(CPPLOGGER_VISIBILITY SHARED)
else()
    set(CPPLOGGER_VISIBILITY STATIC)
endif()

cpplogger_generate_build_config(
    MACRO_NAME
    "CPPLOGGER_API"
    OUTPUT_PATH
    "${CMAKE_CURRENT_BINARY_DIR}/generated/cpploggerBuildConfig.hpp"
    VERSION
    "2.1.1"
    TEMPLATE
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildConfig.hpp.in"
)

add_library(${PROJECT_NAME} ${CPPLOGGER_VISIBILITY} "source/Logger.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC "source/" "${CMAKE_CURRENT_BINARY_DIR}/generated/")
target_compile_options(
    ${PROJECT_NAME}
    PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall
            -Wextra>
            $<$<CXX_COMPILER_ID:MSVC>:
            /W4>
)

target_compile_definitions(${PROJECT_NAME} PUBLIC _CRT_SECURE_NO_WARNINGS)

target_compile_options(
    ${PROJECT_NAME} PUBLIC $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: >
                           $<$<CXX_COMPILER_ID:MSVC>: /Zc:preprocessor>
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(examples/)
endif()
