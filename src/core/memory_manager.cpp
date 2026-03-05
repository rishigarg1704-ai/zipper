#include "core/memory_manager.hpp"
#include <iostream>

#if defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(__linux__)
#include <sys/sysinfo.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace zipper {

MemoryManager& MemoryManager::instance() {
    static MemoryManager inst;
    return inst;
}

std::size_t MemoryManager::get_total_system_memory_bytes() const {

#if defined(__APPLE__)
    int mib[2] = {CTL_HW, HW_MEMSIZE};
    std::size_t size = 0;
    std::size_t len = sizeof(size);
    sysctl(mib, 2, &size, &len, nullptr, 0);
    return size;

#elif defined(__linux__)
    struct sysinfo info;
    sysinfo(&info);
    return static_cast<std::size_t>(info.totalram) * info.mem_unit;

#elif defined(_WIN32)
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return static_cast<std::size_t>(status.ullTotalPhys);

#else
    return 0;
#endif
}

MemoryCheckResult MemoryManager::check(std::size_t estimated_model_bytes) {

    std::size_t total = get_total_system_memory_bytes();
    if (total == 0)
        return MemoryCheckResult::DENY;

    std::size_t hard_limit = total / 2; // 50%

    if (estimated_model_bytes > hard_limit)
        return MemoryCheckResult::DENY;

    return MemoryCheckResult::ALLOW;
}

}