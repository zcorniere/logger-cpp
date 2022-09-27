#include "cpplogger/types/Stream.hpp"
#include "cpplogger/Logger.hpp"

namespace cpplogger
{

Stream::Stream(Logger &log, Level level, const std::string message): logger(log)
{
    buffer.level = std::move(level);
    buffer.content = MessageBuffer::MessageContent{};
    buffer.content->message = std::move(message);
}

Stream::~Stream()
{
    if (buffer.content) buffer.content->stream = stream.str();
    logger.endl(std::move(buffer));
}

}    // namespace cpplogger