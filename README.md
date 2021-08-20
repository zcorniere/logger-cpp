# logger-cpp

A simple thread-safe dependencies-free logger for modern c++ application. It goal is to be simple to use, and light memory usage

## Usage

```cpp
// enable the extern declaration in the header
// suffix this line with _PTR if you want logger to be a pointer
#define LOGGER_EXTERN_DECLARATION
#include <Logger.hpp>

Logger logger(std::cout);

int main(void) {
  logger.start();

  logger.err("Test") << "this is an error message, will be printed in red";
  logger.endl();

  LOGGER_WARN << "This is a warning, that will print the current position";
  LOGGER_ENDL; // same as logger->endl();
}
```
