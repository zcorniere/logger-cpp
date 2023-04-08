#pragma once

#include "cpplogger/formatters/IFormatter.hpp"

namespace cpplogger
{

class DefaultFormatter : public IFormatter
{
public:
    DefaultFormatter();
    ~DefaultFormatter();

    std::string format(const Message &message) override;

private:
    static const char *sPattern;
};

}    // namespace cpplogger
