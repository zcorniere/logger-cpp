#pragma once

#include "cpplogger/sinks/ISink.hpp"

#include <cstdio>
#include <mutex>

namespace cpplogger
{

class StdoutSink : public ISink
{
public:
    StdoutSink(std::FILE *file);

    virtual void write(const Message &message) override;
    virtual void flush() override;
    virtual void SetFormatter(IFormatter *formatter) override;

private:
    IFormatter *p_Formatter = nullptr;
    std::FILE *p_File;
    std::mutex &r_Mutex;
};

}    // namespace cpplogger