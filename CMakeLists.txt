cmake_minimum_required(VERSION 3.0)
project(logger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

option(BUILD_EXAMPLE "Build example application" OFF)

if (BUILD_EXAMPLE)
    add_subdirectory(example/)
endif()

add_library(${PROJECT_NAME} STATIC source/Logger.cpp source/ProgressBar.cpp source/lib.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
target_include_directories(${PROJECT_NAME} PUBLIC include/)

target_compile_options(${PROJECT_NAME} PRIVATE
     $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
          -Wall -Wextra -Werror>
     $<$<CXX_COMPILER_ID:MSVC>:
          /W4 /WX>)
