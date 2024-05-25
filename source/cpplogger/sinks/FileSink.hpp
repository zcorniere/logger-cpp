#pragma once

#include "cpplogger/sinks/ISink.hpp"

#include <cstdio>
#include <filesystem>

namespace cpplogger
{

template <Formatter T>
class FileSink : public TSink<T>
{
public:
    /// @brief Construct a FileSink object from a file path
    /// @param FilePath The path to the file to write to
    FileSink(const std::filesystem::path FilePath, bool AppendToOutput)
        : p_File(fopen(FilePath.string().c_str(), AppendToOutput ? "a" : "w"))
    {
        if (!p_File) { perror("fopen() failed"); }
    }
    /// @brief Construct a FileSink object from a FILE pointer
    /// @param InFile The FILE pointer to write to (will take ownership of the pointer)
    FileSink(FILE *const InFile): p_File(InFile) {}

    virtual ~FileSink()
    {
        if (p_File) { fclose(p_File); }
    }

    virtual void write(const Message &message) override
    {
        // Don't do anything if the file is not valid
        if (!p_File) [[unlikely]] { return; }

        std::string formatter_string = T::format(message) + "\n";

        std::fwrite(formatter_string.data(), sizeof(char), formatter_string.size(), p_File);
    }
    virtual void flush() override { std::fflush(p_File); }

private:
    std::FILE *const p_File;
};

}    // namespace cpplogger
