#pragma once

#include <ostream>

namespace cpplogger
{

class IUpdate
{
public:
    IUpdate() = default;
    IUpdate(const IUpdate &) = delete;
    virtual ~IUpdate() {}
    virtual void update(std::ostream &out) const = 0;
};

}    // namespace cpplogger