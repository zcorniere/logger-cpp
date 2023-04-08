#pragma once

#include "cpplogger/types/Message.hpp"

namespace cpplogger
{

class IFormatter
{
public:
    virtual ~IFormatter() {}

    virtual std::string format(const Message &message) = 0;
};

}    // namespace cpplogger