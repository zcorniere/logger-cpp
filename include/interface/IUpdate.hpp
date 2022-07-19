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
    [[nodiscard]] virtual bool isComplete() const noexcept = 0;
    virtual void update(std::ostream &out) const = 0;
};

}    // namespace cpplogger