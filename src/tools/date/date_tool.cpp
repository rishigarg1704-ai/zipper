#include "date_tool.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace zipper {

std::string DateTool::name() const
{
    return "date";
}

std::string DateTool::description() const
{
    return "Returns the local date or time.";
}

std::string DateTool::execute(const std::map<std::string, std::string>& arguments)
{
    std::string mode = "date";
    const auto it = arguments.find("mode");
    if (it != arguments.end() && !it->second.empty()) {
        mode = it->second;
    }

    const auto now = std::chrono::system_clock::now();
    const std::time_t tt = std::chrono::system_clock::to_time_t(now);

    std::tm local_tm{};
#if defined(_WIN32)
    localtime_s(&local_tm, &tt);
#else
    localtime_r(&tt, &local_tm);
#endif

    std::ostringstream out;
    if (mode == "time") {
        out << std::put_time(&local_tm, "%H:%M:%S");
        return out.str();
    }

    if (mode == "datetime") {
        out << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S");
        return out.str();
    }

    out << std::put_time(&local_tm, "%Y-%m-%d");
    return out.str();
}

} // namespace zipper
