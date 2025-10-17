#pragma once

#include "cpplogger/types/Message.hpp"

namespace cpplogger
{

template <typename T>
concept Formatter = requires() {
    { T::format(typename cpplogger::Message{}) } -> std::convertible_to<std::string>;
};

class ISink
{
public:
    virtual ~ISink() {}
    virtual void write(const Message &message) = 0;
    virtual void flush() = 0;
};

}    // namespace cpplogger
