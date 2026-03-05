#pragma once

#include <string>
#include <fstream>

namespace zipper {

class Logger {
public:
    static Logger& instance();

    void init(const std::string& file);
    void shutdown();

    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void debug(const std::string& message);

private:
    Logger() = default;

    std::ofstream log_file_;
    bool ready_ = false;
};

}