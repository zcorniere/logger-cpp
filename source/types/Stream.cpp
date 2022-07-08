#include "types/Stream.hpp"

#include "Logger.hpp"

namespace cpplogger
{

Stream::Stream(Logger &log, MessageBuffer &buffer, const std::string message): buffer(buffer), logger(log)
{
    buffer.content = MessageBuffer::MessageContent{};
    buffer.content->message = std::move(message);
}

Stream::~Stream()
{
    buffer.content->stream = stream.str();
    logger.endl();
}

}    // namespace cpplogger