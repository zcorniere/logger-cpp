# logger-cpp

[![Linux CI](https://github.com/zcorniere/logger-cpp/actions/workflows/linux_ci.yml/badge.svg)](https://github.com/zcorniere/logger-cpp/actions/workflows/linux_ci.yml)
[![Windows CI](https://github.com/zcorniere/logger-cpp/actions/workflows/windows_ci.yml/badge.svg)](https://github.com/zcorniere/logger-cpp/actions/workflows/windows_ci.yml)

A simple thread-safe dependencies-free logger for modern c++ application. 

It is designed to be simple to use, fast, and safe to use in a multi-threaded context.

## Example

[![asciicast](https://asciinema.org/a/510046.svg)](https://asciinema.org/a/510046)

## Requirements
- a C++ 20 capable compiler
- CMake version > 3.0

### CMake configuration
```cmake
# add this repo to cmake, will add a target named "logger"
add_subdirectory(${LOGGER_SOURCE_DIR})

# Optional: Add a compile definition to enable global `cpplogger::Logger object`
target_compile_definitions(logger PUBLIC LOGGER_EXTERN_DECLARATION)
# or 
target_compile_definitions(logger PUBLIC LOGGER_EXTERN_DECLARATION_PTR)

# add the logger as a dependency of your project
target_link_libraries(${PROJECT_NAME} PUBIC logger)
```

#### CMake options
- `BUILD_EXAMPLE`: Build the exemples
- `BUILD_BENCHMARK`: Build the benchmark

## Usage
See the [example](./example/) folder.