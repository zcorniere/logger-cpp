#pragma once

#include <mutex>

namespace cpplogger::internal
{

struct ConsoleMutex {
    inline static std::mutex &mutex()
    {
        static std::mutex s_mutex;
        return s_mutex;
    }
};

}    // namespace cpplogger::internal