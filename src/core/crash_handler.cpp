#include "core/crash_handler.hpp"
#include "core/logger.hpp"

#include <csignal>
#include <cstdlib>
#include <exception>

namespace zipper {

namespace {

void signal_handler(int signal) {
    Logger::instance().log(LogLevel::ERROR, "Fatal signal received: " + std::to_string(signal));
    Logger::instance().shutdown();
    std::_Exit(EXIT_FAILURE);
}

void terminate_handler() {
    Logger::instance().log(LogLevel::ERROR, "Unhandled exception — std::terminate called");
    Logger::instance().shutdown();
    std::_Exit(EXIT_FAILURE);
}

} // anonymous namespace

void CrashHandler::install() {
    std::signal(SIGSEGV, signal_handler);
    std::signal(SIGABRT, signal_handler);
    std::set_terminate(terminate_handler);

    Logger::instance().log(LogLevel::INFO, "CrashHandler installed");
}

}