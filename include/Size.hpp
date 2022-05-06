#pragma once

namespace cpplogger
{

struct Size {
    int columns = 0;
    int lines = 0;

    static Size get() noexcept;
};

}    // namespace logger