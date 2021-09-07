cmake_minimum_required(VERSION 3.20)
project(examples)

add_executable(example_noglobal example_noglobal.cpp)
target_link_libraries(example_noglobal PRIVATE logger)

add_executable(example_multithread example_multithread.cpp)
target_link_libraries(example_multithread PRIVATE logger)

add_executable(example_level example_level.cpp)
target_link_libraries(example_level PRIVATE logger)

add_executable(example_simple example_simple.cpp)
target_link_libraries(example_simple PRIVATE logger)

add_executable(example example.cpp)
target_link_libraries(example PRIVATE logger)
