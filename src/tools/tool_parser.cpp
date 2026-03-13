#include "tool_parser.hpp"

#include <algorithm>
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

    if (trimmed.front() != '/') {
        return false;
    }

    std::stringstream ss(trimmed.substr(1));
    std::string command;

    ss >> command;
    if (command.empty()) {
        return false;
    }

    command = toLowerCopy(command);

    std::string remainder;
    std::getline(ss, remainder);
    remainder = ltrim(remainder);
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
        arguments["symbol"] = remainder;
        return true;
    }

    if (command == "search" || command == "web_search" || command == "websearch" ||
        command == "find" || command == "lookup")
    {
        toolName = "web_search";
        arguments["query"] = remainder;
        return true;
    }

    if (command == "weather") {
        toolName = "web_search";
        arguments["query"] = remainder.empty() ? "weather" : "weather " + remainder;
        return true;
    }

    return false;
}

} // namespace zipper
