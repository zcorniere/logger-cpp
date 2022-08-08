cmake_minimum_required(VERSION 3.0)
project(logger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

option(BUILD_EXAMPLE "Build example application" OFF)
if(BUILD_EXAMPLE)
    message(STATUS "Building example application")
    add_subdirectory(example/)
endif()

set(LOGGER_DELAY 100ms)

set(LOGGER_LIBRARY_TYPE
    "${LOGGER_LIBRARY_TYPE}"
    CACHE STRING "Library type override for GLFW (SHARED, STATIC, OBJECT, or empty to follow BUILD_SHARED_LIBS)"
)
message(STATUS "Build ${PROJECT_NAME} as a ${LOGGER_LIBRARY_TYPE} library")

add_library(
    ${PROJECT_NAME}
    ${LOGGER_LIBRARY_TYPE}
    source/lib.cpp
    source/Size.cpp
    source/Logger.cpp
    source/handlers.cpp
    source/types/Level.cpp
    source/types/Stream.cpp
    source/types/Spinner.cpp
    source/types/ProgressBar.cpp
    source/types/MessageBuffer.cpp
)

if(${LOGGER_LIBRARY_TYPE} STREQUAL "SHARED")
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_compile_definitions(${PROJECT_NAME} PUBLIC LOGGER_DELAY=${LOGGER_DELAY})
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
target_include_directories(${PROJECT_NAME} PUBLIC include/)

target_compile_options(
    ${PROJECT_NAME}
    PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall
            -Wextra>
            $<$<CXX_COMPILER_ID:MSVC>:
            /W4>
)

option(BUILD_BENCHMARK "Build benchmark application" OFF)
if(BUILD_BENCHMARK)
    message(STATUS "Building benchmark application")
    add_executable(benchmark benchmark/main.cpp)
    target_link_libraries(benchmark PUBLIC logger)
endif()
