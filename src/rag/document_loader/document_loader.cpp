#include "document_loader.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>

namespace zipper::rag {
namespace {

std::string to_lower_copy(std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string trim_copy(const std::string& value)
{
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

bool is_supported_extension(const std::filesystem::path& path)
{
    const std::string ext = to_lower_copy(path.extension().string());
    return ext == ".txt" || ext == ".md";
}

std::string read_file(const std::filesystem::path& path)
{
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return {};
    }

    std::ostringstream stream;
    stream << in.rdbuf();
    return stream.str();
}

} // namespace

DocumentLoadResult DocumentLoader::load_document(const std::string& path) const
{
    DocumentLoadResult result;

    const std::string cleaned = trim_copy(path);
    if (cleaned.empty()) {
        result.error = "No document path provided.";
        return result;
    }

    const std::filesystem::path input_path(cleaned);
    std::vector<std::filesystem::path> candidates;
    candidates.push_back(input_path);

    if (input_path.is_relative()) {
        candidates.push_back(std::filesystem::path("..") / input_path);
    }

    std::filesystem::path resolved;
    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate)) {
            resolved = candidate;
            break;
        }
    }

    if (resolved.empty()) {
        result.error = "File not found: " + cleaned;
        return result;
    }

    if (std::filesystem::is_directory(resolved)) {
        result.error = "Path is a directory: " + resolved.string();
        return result;
    }

    if (!is_supported_extension(resolved)) {
        result.error = "Unsupported format. Only .txt and .md are supported.";
        return result;
    }

    const std::string content = read_file(resolved);
    if (content.empty()) {
        result.error = "Document is empty or unreadable: " + resolved.string();
        return result;
    }

    result.success = true;
    result.resolved_path = resolved.string();
    result.text = content;
    return result;
}

} // namespace zipper::rag
