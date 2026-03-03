#pragma once

#include <string>
#include <fstream>
#include <mutex>

namespace zipper {

enum class LogLevel {
    INFO,
    WARN,
    ERROR,
    DEBUG
};

class Logger {
public:
    static Logger& instance();

    void init(const std::string& filename);
    void shutdown();

    void log(LogLevel level, const std::string& message);

private:
    Logger() = default;
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream log_file_;
    std::mutex mutex_;
    bool initialized_ = false;

    std::string level_to_string(LogLevel level);
    std::string current_timestamp();
};

}