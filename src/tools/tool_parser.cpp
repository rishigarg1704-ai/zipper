#include "tool_parser.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <sstream>

namespace zipper {
namespace {

std::string toLowerCopy(std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); }
    );
    return value;
}

std::string ltrim(const std::string& value)
{
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }
    return value.substr(start);
}

bool startsWith(const std::string& text, const std::string& prefix)
{
    return text.size() >= prefix.size() &&
           std::equal(prefix.begin(), prefix.end(), text.begin());
}

template <std::size_t N>
bool containsAny(const std::string& text, const std::array<const char*, N>& phrases)
{
    for (const char* phrase : phrases) {
        if (text.find(phrase) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} // namespace

bool ToolParser::parse(
    const std::string& input,
    std::string& toolName,
    std::map<std::string, std::string>& arguments)
{
    toolName.clear();
    arguments.clear();

    const std::string trimmed = ltrim(input);
    if (trimmed.empty()) {
        return false;
    }

    std::stringstream ss(trimmed);
    std::string command;

    ss >> command;
    command = toLowerCopy(command);

    std::string remainder;
    std::getline(ss, remainder);
    remainder = ltrim(remainder);

    const std::string lowered = toLowerCopy(trimmed);
    const std::string remainder_lower = toLowerCopy(remainder);

    if (command == "calculate" || command == "calc") {
        toolName = "calculator";
        arguments["expression"] = remainder;
        return true;
    }

    if (command == "date" || command == "today") {
        toolName = "date";
        if (remainder_lower == "time" || remainder_lower == "current time") {
            arguments["mode"] = "time";
        } else if (remainder_lower == "datetime" || remainder_lower == "date time") {
            arguments["mode"] = "datetime";
        } else {
            arguments["mode"] = "date";
        }
        return true;
    }

    if (command == "time" || command == "now") {
        toolName = "date";
        arguments["mode"] = "time";
        return true;
    }

    if (command == "datetime") {
        toolName = "date";
        arguments["mode"] = "datetime";
        return true;
    }

    if (command == "sysinfo" || command == "system" || command == "systeminfo") {
        toolName = "system_info";
        return true;
    }

    if (command == "read" || command == "cat") {
        toolName = "file_reader";
        arguments["path"] = remainder;
        return true;
    }

    if (command == "stock" || command == "quote" || command == "ticker") {
        toolName = "stock";
        arguments["query"] = remainder;
        return true;
    }

    if (command == "search" || command == "web_search" || command == "websearch" ||
        command == "find" || command == "lookup")
    {
        toolName = "web_search";
        arguments["query"] = remainder;
        return true;
    }

    if (containsAny(lowered, std::array<const char*, 5>{
            "current date and time",
            "date and time",
            "current datetime",
            "today date and time",
            "today's date and time"}))
    {
        toolName = "date";
        arguments["mode"] = "datetime";
        return true;
    }

    if (containsAny(lowered, std::array<const char*, 6>{
            "today date",
            "today's date",
            "current date",
            "what is the date",
            "what's the date",
            "date today"}))
    {
        toolName = "date";
        arguments["mode"] = "date";
        return true;
    }

    if (containsAny(lowered, std::array<const char*, 6>{
            "current time",
            "time now",
            "local time",
            "what time is it",
            "what's the time",
            "time right now"}))
    {
        toolName = "date";
        arguments["mode"] = "time";
        return true;
    }

    if (containsAny(lowered, std::array<const char*, 5>{
            "system info",
            "system information",
            "device info",
            "machine info",
            "os info"}))
    {
        toolName = "system_info";
        return true;
    }

    const std::array<std::string, 4> file_prefixes = {
        "read file ",
        "read ",
        "cat ",
        "open file "
    };

    for (const std::string& prefix : file_prefixes) {
        if (startsWith(lowered, prefix)) {
            std::string path = ltrim(trimmed.substr(prefix.size()));
            if (!path.empty()) {
                toolName = "file_reader";
                arguments["path"] = path;
                return true;
            }
        }
    }

    if (lowered.find(" stock") != std::string::npos ||
        startsWith(lowered, "stock ") ||
        lowered.find("share price") != std::string::npos ||
        lowered.find("ticker") != std::string::npos)
    {
        toolName = "stock";
        arguments["query"] = trimmed;
        return true;
    }

    const std::array<std::string, 8> natural_search_prefixes = {
        "search for ",
        "search ",
        "web search ",
        "look up ",
        "please search ",
        "can you search ",
        "could you search ",
        "find "
    };

    for (const std::string& prefix : natural_search_prefixes) {
        if (startsWith(lowered, prefix)) {
            std::string query = ltrim(trimmed.substr(prefix.size()));
            if (!query.empty()) {
                toolName = "web_search";
                arguments["query"] = query;
                return true;
            }
        }
    }

    const std::array<std::string, 4> dynamic_search_hints = {
        "latest",
        "news",
        "weather",
        "headlines"
    };

    for (const std::string& hint : dynamic_search_hints) {
        if (lowered.find(hint) != std::string::npos) {
            toolName = "web_search";
            arguments["query"] = trimmed;
            return true;
        }
    }

    return false;
}

} // namespace zipper
