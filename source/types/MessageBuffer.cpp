#include "cpplogger/types/MessageBuffer.hpp"
#include "cpplogger/utils/sequences.hpp"

#include <iomanip>

namespace cpplogger
{

std::ostream &operator<<(std::ostream &os, const MessageBuffer &buffer)
{
    if (buffer.content) {
        const std::time_t time = std::chrono::system_clock::to_time_t(buffer.now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(buffer.now.time_since_epoch()) -
                  std::chrono::duration_cast<std::chrono::seconds>(buffer.now.time_since_epoch());

        os << "[" << color(levelColor(buffer.level)) << std::put_time(std::localtime(&time), "%F %T") << ":"
           << std::setfill('0') << std::setw(3) << ms.count() << reset() << ']';
        os << "[" << color(levelColor(buffer.level)) << to_string(buffer.level) << reset() << ']';
        if (!buffer.content->message.empty()) {
            os << "[" << color(levelColor(buffer.level)) << buffer.content->message << reset() << ']';
        }
        os << ' ' << buffer.content->stream;
    }
    return os;
}

}    // namespace cpplogger