#pragma once

#include <chrono>
#include <optional>
#include <string>

#include "types/Level.hpp"

namespace cpplogger
{

struct MessageBuffer {
    struct MessageContent {
        std::string stream;
        std::string message;
    };
    Level level = Level::Message;
    std::optional<MessageContent> content = std::nullopt;
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    friend std::ostream &operator<<(std::ostream &os, const MessageBuffer &buffer);
};

}    // namespace cpplogger