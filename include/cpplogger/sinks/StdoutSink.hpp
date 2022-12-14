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

    void write(const Message &message) override;
    void flush() override;

private:
    std::FILE *p_File;
    std::mutex &r_Mutex;
};

}    // namespace cpplogger