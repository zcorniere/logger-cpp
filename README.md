# logger-cpp

A simple thread-safe logger for modern c++ application

## Usage

```cpp
#define LOGGER_EXTERN_DECLARATION_PTR
#include <Logger.hpp>

Logger *logger = nullptr;

int main(void) {
  logger = new Logger(std::cout);
  logger.start();

  logger->err("Test") << "this is an error message, will be printed in red";
  logger->endl();

  LOGGER_WARN << "This is a warning, that will print the current position";
  LOGGER_ENDL; // same as logger->endl();

  delete logger;
}
```
