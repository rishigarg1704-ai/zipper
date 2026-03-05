#pragma once

#include <cstddef>

namespace zipper {

enum class MemoryCheckResult {
    ALLOW,
    DENY
};

class MemoryManager {
public:
    static MemoryManager& instance();

    MemoryCheckResult check(std::size_t estimated_model_bytes);

private:
    MemoryManager() = default;

    std::size_t get_total_system_memory_bytes() const;
};

}