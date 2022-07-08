#include "Logger.hpp"

#include "utils/demangle.hpp"

static cpplogger::Logger *handler_logger = nullptr;
static const std::terminate_handler static_handler = std::get_terminate();
static std::terminate_handler previous_handler = nullptr;

void backstop()
{
    auto const ep = std::current_exception();
    if (ep && handler_logger) {
        {
            auto stream = handler_logger->err("Terminate");
            try {
                stream << "Terminating with uncaught exception of type `"
                       << cpplogger::demangle_exception_ptr(ep).value() << "`";
                std::rethrow_exception(ep);

            } catch (const std::exception &e) {
                stream << " with `what()` = \"" << e.what() << "\"";
            } catch (...) {
                std::abort();
            }
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
    return 0;
}

namespace cpplogger
{

void Logger::init()
{
    handler_logger = this;

    /// previous_handler is normaly the default handler. If set_terminate is called before starting the logger, they
    /// will be different.
    if (static_handler != std::get_terminate()) { previous_handler = std::get_terminate(); }
    std::set_terminate(backstop);
}

void Logger::deinit()
{
    if (backstop == std::get_terminate()) std::set_terminate(static_handler);
    handler_logger = nullptr;
}

}    // namespace cpplogger