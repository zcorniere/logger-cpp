#pragma once

#include "cpplogger/formatters/IFormatter.hpp"
#include "cpplogger/types/Message.hpp"

#include <memory>

namespace cpplogger
{

class ISink
{
public:
    virtual ~ISink() {}

    virtual void write(const Message &message) = 0;
    virtual void flush() = 0;

    virtual void SetFormatter(std::unique_ptr<IFormatter> formatter) = 0;
};

}    // namespace cpplogger
