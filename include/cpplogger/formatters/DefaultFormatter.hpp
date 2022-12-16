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
    std::string m_Pattern = "[{LogTime:%F} {LogTime:%T}:{LogTimeMilis:03}][{LogLevel}][{Category}] {Message}";
};

}    // namespace cpplogger