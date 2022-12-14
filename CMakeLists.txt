cmake_minimum_required(VERSION 3.11)
include(FetchContent)
project(logger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CPPLOGGER_USE_STD_FORMAT OFF "Should use the standard std::format")

find_package(Threads REQUIRED)

add_library(${PROJECT_NAME} STATIC source/lib.cpp source/sinks/StdoutSink.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
target_include_directories(${PROJECT_NAME} PUBLIC include/)
target_compile_definitions(${PROJECT_NAME} PUBLIC CPPLOGGER_USE_STD_FORMAT=${CPPLOGGER_USE_STD_FORMAT})

target_compile_options(
    ${PROJECT_NAME}
    PRIVATE $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall
            -Wextra>
            $<$<CXX_COMPILER_ID:MSVC>:
            /W4>
)


if(NOT CPPLOGGER_USE_STD_FORMAT)
    fetchcontent_declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 9.1.0
    )
    if(NOT catch2_POPULATED)
        message(STATUS "Populating fmt")
        fetchcontent_populate(fmt)
        add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
    endif()
    target_link_libraries(${PROJECT_NAME} PUBLIC fmt)
endif()