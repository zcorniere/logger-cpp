#pragma once

#include "cpplogger/sinks/ISink.hpp"

#include <cstdio>
#include <mutex>

namespace cpplogger
{

class StdoutSink : public ISink
{
private:
    static unsigned initialize_terminal(std::FILE *p_File);

public:
    StdoutSink(std::FILE *file);

    virtual void write(const Message &message) override;
    virtual void flush() override;
    virtual void SetFormatter(std::unique_ptr<IFormatter> formatter) override;

private:
    std::unique_ptr<IFormatter> p_Formatter = nullptr;
    std::FILE *p_File;
    std::mutex &r_Mutex;
};

}    // namespace cpplogger
