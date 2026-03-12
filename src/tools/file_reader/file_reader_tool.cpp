#include "file_reader_tool.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace zipper {
namespace {

std::size_t parseMaxChars(const std::map<std::string, std::string>& arguments)
{
    constexpr std::size_t kDefaultMaxChars = 4000;
    constexpr std::size_t kMaxAllowedChars = 50000;

    const auto it = arguments.find("max_chars");
    if (it == arguments.end() || it->second.empty()) {
        return kDefaultMaxChars;
    }

    try {
        const std::size_t requested = static_cast<std::size_t>(std::stoull(it->second));
        if (requested == 0) {
            return kDefaultMaxChars;
        }
        return requested > kMaxAllowedChars ? kMaxAllowedChars : requested;
    } catch (...) {
        return kDefaultMaxChars;
    }
}

std::filesystem::path resolvePath(const std::string& raw_path)
{
    const std::filesystem::path direct(raw_path);
    if (std::filesystem::exists(direct)) {
        return direct;
    }

    if (direct.is_relative()) {
        const std::filesystem::path parent = std::filesystem::path("..") / direct;
        if (std::filesystem::exists(parent)) {
            return parent;
        }
    }

    return direct;
}

} // namespace

std::string FileReaderTool::name() const
{
    return "file_reader";
}

std::string FileReaderTool::description() const
{
    return "Reads a local text file.";
}

std::string FileReaderTool::execute(const std::map<std::string, std::string>& arguments)
{
    auto path_it = arguments.find("path");
    if (path_it == arguments.end() || path_it->second.empty()) {
        path_it = arguments.find("file");
    }

    if (path_it == arguments.end() || path_it->second.empty()) {
        return "No file path provided. Example: read src/main.cpp";
    }

    const std::filesystem::path path = resolvePath(path_it->second);

    if (!std::filesystem::exists(path)) {
        return "File not found: " + path_it->second;
    }

    if (std::filesystem::is_directory(path)) {
        return "Path is a directory, not a file: " + path.string();
    }

    std::ifstream input(path, std::ios::binary);
    if (!input) {
        return "Unable to open file: " + path.string();
    }

    std::ostringstream content_stream;
    content_stream << input.rdbuf();
    std::string content = content_stream.str();

    if (content.empty()) {
        return "File is empty: " + path.string();
    }

    const std::size_t max_chars = parseMaxChars(arguments);
    if (content.size() <= max_chars) {
        return content;
    }

    return content.substr(0, max_chars) +
           "\n\n[Truncated to " + std::to_string(max_chars) + " characters]";
}

} // namespace zipper
