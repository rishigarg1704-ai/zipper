#include "core/crash_handler.hpp"
#include "core/logger.hpp"

#include <csignal>
#include <exception>

namespace zipper {

namespace {

void handle_signal(int sig) {
    Logger::instance().error("Fatal signal: " + std::to_string(sig));
    std::_Exit(EXIT_FAILURE);
}

void handle_terminate() {
    Logger::instance().error("Unhandled exception");
    std::_Exit(EXIT_FAILURE);
}

}

void CrashHandler::install() {
    std::signal(SIGINT, handle_signal);
    std::signal(SIGTERM, handle_signal);
    std::set_terminate(handle_terminate);
}

}