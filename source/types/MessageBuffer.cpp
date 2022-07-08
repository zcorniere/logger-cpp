#include "types/MessageBuffer.hpp"

#include <iomanip>

namespace cpplogger
{

std::ostream &operator<<(std::ostream &os, const MessageBuffer &buffer)
{
    if (buffer.content) {
        const std::time_t time = std::chrono::high_resolution_clock::to_time_t(buffer.now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(buffer.now.time_since_epoch()) -
                  std::chrono::duration_cast<std::chrono::seconds>(buffer.now.time_since_epoch());
        auto micro = std::chrono::duration_cast<std::chrono::microseconds>(buffer.now.time_since_epoch()) -
                     std::chrono::duration_cast<std::chrono::milliseconds>(buffer.now.time_since_epoch());

        os << "[" << color(levelColor(buffer.level)) << std::put_time(std::localtime(&time), "%OM:%OS") << ":"
           << ms.count() << ":" << micro.count() << reset() << ']';
        os << "[" << color(levelColor(buffer.level)) << to_string(buffer.level) << reset() << ']';
        if (!buffer.content->message.empty()) {
            os << "[" << color(levelColor(buffer.level)) << buffer.content->message << reset() << ']';
        }
        os << ' ' << buffer.content->stream;
    }
    return os;
}

}    // namespace cpplogger