#include "Logger.hpp"

static cpplogger::Logger *handler_logger = nullptr;
static const std::terminate_handler static_handler = std::get_terminate();
static std::terminate_handler previous_handler = nullptr;

void backstop()
{
    auto const ep = std::current_exception();
    if (ep && handler_logger) {
        auto stream = handler_logger->err("Terminate");
        try {
            int status;
            auto const etype = abi::__cxa_demangle(abi::__cxa_current_exception_type()->name(), 0, 0, &status);
            stream << "Terminating with uncaught exception of type `" << etype << "`";
            std::rethrow_exception(ep);

        } catch (const std::exception &e) {
            stream << " with `what()` = \"" << e.what() << "\"";
        } catch (...) {
        }
        handler_logger->stop();
    }
    if (previous_handler) previous_handler();
    std::abort();
}

unsigned init_terminal()
{
#if defined(TERMINAL_TARGET_WINDOWS)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return GetLastError();
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return GetLastError();
    dwMode |= DWORD(ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    if (!SetConsoleMode(hOut, dwMode)) return GetLastError();
#elif defined(TERMINAL_TARGET_POSIX)
#endif

    /// previous_handler is normaly the default handler. If set_terminate is called before starting the logger, they
    /// will be different.
    return 0;
}

namespace cpplogger
{

void Logger::init()
{
    handler_logger = this;
    if (static_handler != std::get_terminate()) { previous_handler = std::get_terminate(); }
    std::set_terminate(backstop);
}

void Logger::deinit()
{
    std::set_terminate(static_handler);
    handler_logger = nullptr;
}

}    // namespace cpplogger