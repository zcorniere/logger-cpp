cmake_minimum_required(VERSION 3.0)
project(logger)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(${PROJECT_NAME} source/Logger.cpp source/ProgressBar.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC include/)
