#include "core/logger.hpp"
#include <iostream>

namespace zipper {

Logger& Logger::instance() {
    static Logger inst;
    return inst;
}

void Logger::init(const std::string& file) {
    log_file_.open(file, std::ios::app);
    ready_ = log_file_.is_open();
}

void Logger::shutdown() {
    if (log_file_.is_open())
        log_file_.close();
}

void Logger::info(const std::string& message) {
    if (ready_)
        log_file_ << "[INFO] " << message << "\n";
}

void Logger::warn(const std::string& message) {
    if (ready_)
        log_file_ << "[WARN] " << message << "\n";
}

void Logger::error(const std::string& message) {
    if (ready_)
        log_file_ << "[ERROR] " << message << "\n";
}

void Logger::debug(const std::string& message) {
    if (ready_)
        log_file_ << "[DEBUG] " << message << "\n";
}

}