#pragma once

#include <string>
#include <cstddef>

namespace zipper {

enum class MemoryCheckResult {
    ALLOW,
    REQUIRE_OVERRIDE,
    REFUSE
};

class MemoryManager {
public:
    MemoryManager();

    MemoryCheckResult can_load_model(const std::string& model_path);

    void enable_override();
    void reset_override();

    std::size_t total_ram_bytes() const;

private:
    std::size_t total_ram_;
    bool override_enabled_;

    std::size_t get_total_system_ram();
    std::size_t get_file_size(const std::string& path);

    std::size_t strict_cap_bytes() const;     // 50%
    std::size_t override_cap_bytes() const;   // 75%
};

}