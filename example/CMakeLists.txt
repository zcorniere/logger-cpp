cmake_minimum_required(VERSION 3.20)
project(examples)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(example_noglobal example_noglobal.cpp)
target_link_libraries(example_noglobal PRIVATE logger)

add_executable(example_ptr example_ptr.cpp)
target_link_libraries(example_ptr PRIVATE logger)

add_executable(example_level example_level.cpp)
target_link_libraries(example_level PRIVATE logger)

add_executable(example_simple example_simple.cpp)
target_link_libraries(example_simple PRIVATE logger)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE logger)
