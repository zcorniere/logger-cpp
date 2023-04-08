#pragma once

#include "cpplogger/formatters/IFormatter.hpp"

namespace cpplogger
{

class ColorFormatter : public IFormatter
{
public:
    ColorFormatter();
    ~ColorFormatter();

    std::string format(const Message &message) override;

private:
    static const char *sPattern;
};

}    // namespace cpplogger
