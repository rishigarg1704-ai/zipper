#include "system_info_tool.hpp"

#include <filesystem>
#include <sstream>
#include <thread>

namespace zipper {
namespace {

std::string detectOsName()
{
#if defined(_WIN32)
    return "Windows";
#elif defined(__APPLE__) && defined(__MACH__)
    return "macOS";
#elif defined(__linux__)
    return "Linux";
#else
    return "Unknown";
#endif
}

std::string detectArchitecture()
{
#if defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)
    return "arm64";
#elif defined(__x86_64__) || defined(_M_X64)
    return "x86_64";
#elif defined(__i386__) || defined(_M_IX86)
    return "x86";
#else
    return "unknown";
#endif
}

} // namespace

std::string SystemInfoTool::name() const
{
    return "system_info";
}

std::string SystemInfoTool::description() const
{
    return "Returns basic system information.";
}

std::string SystemInfoTool::execute(const std::map<std::string, std::string>& /*arguments*/)
{
    std::ostringstream out;
    out << "OS: " << detectOsName() << '\n';
    out << "Architecture: " << detectArchitecture() << '\n';
    out << "Hardware Threads: " << std::thread::hardware_concurrency() << '\n';
    out << "Working Directory: " << std::filesystem::current_path().string();
    return out.str();
}

} // namespace zipper
