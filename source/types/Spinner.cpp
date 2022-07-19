#include "cpplogger/types/Spinner.hpp"

namespace cpplogger
{

Spinner::Spinner(const std::string &message, const Spinner::Style style): message(message), style(std::move(style)) {}

void Spinner::update(std::ostream &out) const
{
    if (isComplete())
        out << style.completion;
    else
        out << style.style.at(index);

    out << style.separator << message << std::endl;
    index = (index + 1) % style.style.size();
}

}    // namespace cpplogger