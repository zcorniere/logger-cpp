#pragma once

#include "cpplogger/types/Message.hpp"

namespace cpplogger
{

class ISink
{
public:
    virtual ~ISink() {}

    virtual void write(const Message &message) = 0;
    virtual void flush() = 0;
};

}    // namespace cpplogger