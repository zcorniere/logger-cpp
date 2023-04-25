#pragma once

#include "cpplogger/types/Message.hpp"

#include <memory>

namespace cpplogger
{

template <typename T>
concept Formatter = requires() {
                        {
                            T::format(typename cpplogger::Message{})
                        };
                    };

class ISink
{
public:
    virtual ~ISink() {}
    virtual void write(const Message &message) = 0;
    virtual void flush() = 0;
};

template <Formatter TForm>
class TSink : public ISink
{
};

}    // namespace cpplogger
