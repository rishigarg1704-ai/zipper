#include "core/logger.hpp"

#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace zipper {

Logger& Logger::instance() {
    static Logger instance;
    return instance;
}

Logger::~Logger() {
    shutdown();
}

void Logger::init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (initialized_) {
        return;
    }

    log_file_.open(filename, std::ios::out | std::ios::app);

    if (!log_file_.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        return;
    }

    initialized_ = true;

    // Direct write (NO recursive log() call)
    log_file_
        << "[" << current_timestamp() << "] "
        << "[INFO] "
        << "Logger initialized"
        << std::endl;

    log_file_.flush();
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_) return;

    if (log_file_.is_open()) {
        log_file_
            << "[" << current_timestamp() << "] "
            << "[INFO] "
            << "Logger shutting down"
            << std::endl;

        log_file_.flush();
        log_file_.close();
    }

    initialized_ = false;
}

void Logger::log(LogLevel level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (!initialized_ || !log_file_.is_open()) return;

    log_file_
        << "[" << current_timestamp() << "] "
        << "[" << level_to_string(level) << "] "
        << message
        << std::endl;

    log_file_.flush();
}

std::string Logger::level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::DEBUG: return "DEBUG";
        default:              return "UNKNOWN";
    }
}

std::string Logger::current_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);

    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

} // namespace zipper