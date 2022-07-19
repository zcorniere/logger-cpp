#pragma once

#include "cpplogger/types/MessageBuffer.hpp"
#include "cpplogger/utils/concepts.hpp"

#include <sstream>

namespace cpplogger
{

class Logger;

class Stream
{
public:
    explicit Stream(Logger &log, MessageBuffer &buffer, const std::string message);
    Stream(const Stream &) = delete;
    Stream(const Stream &&) = delete;
    ~Stream();

    inline Stream &operator<<(const Printable auto &object)
    {
        stream << object;
        return *this;
    }

    inline Stream &operator<<(const PrintableIterator auto &container)
    {
        auto size = container.size() - 1;
        stream << "[";
        for (const auto &i: container) {
            stream << '\"';
            (*this) << i;
            stream << '\"';
            if (size-- > 0) stream << ", ";
        }
        stream << "]";
        return *this;
    }

private:
    std::ostringstream stream;
    MessageBuffer &buffer;
    Logger &logger;
};

}    // namespace cpplogger