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
    std::string m_Pattern =
        "[{LogTime:%F} {LogTime:%T}:{LogTimeMilis:03}][{Color}{LogLevel}{Reset}][{Category}] {Message}";
};

}    // namespace cpplogger
