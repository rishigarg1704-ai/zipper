#include "core/memory_manager.hpp"
#include "core/logger.hpp"

#include <sys/types.h>
#include <sys/sysctl.h>
#include <fstream>

namespace zipper {

MemoryManager::MemoryManager()
    : total_ram_(get_total_system_ram()),
      override_enabled_(false)
{
    Logger::instance().log(LogLevel::INFO,
        "Total system RAM detected: " + std::to_string(total_ram_ / (1024 * 1024)) + " MB");
}

std::size_t MemoryManager::get_total_system_ram() {
    int mib[2] = {CTL_HW, HW_MEMSIZE};
    std::uint64_t size = 0;
    std::size_t len = sizeof(size);

    if (sysctl(mib, 2, &size, &len, nullptr, 0) == 0) {
        return static_cast<std::size_t>(size);
    }

    Logger::instance().log(LogLevel::ERROR, "Failed to detect system RAM");
    return 0;
}

std::size_t MemoryManager::get_file_size(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        Logger::instance().log(LogLevel::ERROR, "Failed to open model file: " + path);
        return 0;
    }
    return static_cast<std::size_t>(file.tellg());
}

std::size_t MemoryManager::strict_cap_bytes() const {
    return total_ram_ * 50 / 100;
}

std::size_t MemoryManager::override_cap_bytes() const {
    return total_ram_ * 75 / 100;
}

MemoryCheckResult MemoryManager::can_load_model(const std::string& model_path) {
    std::size_t model_size = get_file_size(model_path);

    Logger::instance().log(LogLevel::INFO,
        "Model size: " + std::to_string(model_size / (1024 * 1024)) + " MB");

    if (model_size == 0) {
        return MemoryCheckResult::REFUSE;
    }

    if (model_size <= strict_cap_bytes()) {
        return MemoryCheckResult::ALLOW;
    }

    if (model_size <= override_cap_bytes()) {
        if (override_enabled_) {
            Logger::instance().log(LogLevel::WARN,
                "Override enabled — allowing high memory load");
            return MemoryCheckResult::ALLOW;
        }
        return MemoryCheckResult::REQUIRE_OVERRIDE;
    }

    return MemoryCheckResult::REFUSE;
}

void MemoryManager::enable_override() {
    override_enabled_ = true;
    Logger::instance().log(LogLevel::WARN,
        "Memory override enabled for this session");
}

void MemoryManager::reset_override() {
    override_enabled_ = false;
}

std::size_t MemoryManager::total_ram_bytes() const {
    return total_ram_;
}

}