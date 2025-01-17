cmake_minimum_required(VERSION 3.11)
project(cpplogger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(
    ${PROJECT_NAME} STATIC
    source/Logger.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC source/)
    
option(CPPLOGGER_NO_EXCEPTIONS ON)
if (CPPLOGGER_NO_EXCEPTIONS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC CPPLOGGER_NO_EXCEPTIONS=1)
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC CPPLOGGER_NO_EXCEPTIONS=0)
endif(CPPLOGGER_NO_EXCEPTIONS)

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
    ${PROJECT_NAME}
    PUBLIC $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            >
            $<$<CXX_COMPILER_ID:MSVC>:
            /Zc:preprocessor>
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_subdirectory(examples/)
endif()
