// src/tools/web_search/web_search_tool.cpp
#include "web_search_tool.hpp"

#include <array>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace zipper {
namespace {

std::string urlEncode(const std::string& value)
{
    std::ostringstream encoded;
    encoded << std::hex << std::uppercase;

    for (unsigned char c : value) {
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded << c;
        } else if (c == ' ') {
            encoded << "%20";
        } else {
            encoded << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(c);
        }
    }

    return encoded.str();
}

std::string urlDecode(const std::string& value)
{
    std::string decoded;
    decoded.reserve(value.size());

    for (std::size_t i = 0; i < value.size(); ++i) {
        char c = value[i];

        if (c == '%' && i + 2 < value.size() &&
            std::isxdigit(static_cast<unsigned char>(value[i + 1])) &&
            std::isxdigit(static_cast<unsigned char>(value[i + 2])))
        {
            const std::string hex = value.substr(i + 1, 2);
            decoded += static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
            i += 2;
            continue;
        }

        if (c == '+') {
            decoded += ' ';
            continue;
        }

        decoded += c;
    }

    return decoded;
}

std::string trim(const std::string& value)
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

void replaceAll(std::string& value, const std::string& from, const std::string& to)
{
    if (from.empty()) {
        return;
    }

    std::size_t pos = 0;
    while ((pos = value.find(from, pos)) != std::string::npos) {
        value.replace(pos, from.size(), to);
        pos += to.size();
    }
}

std::string htmlDecode(std::string value)
{
    replaceAll(value, "&amp;", "&");
    replaceAll(value, "&quot;", "\"");
    replaceAll(value, "&#39;", "'");
    replaceAll(value, "&lt;", "<");
    replaceAll(value, "&gt;", ">");
    return value;
}

std::string stripTags(const std::string& value)
{
    std::string plain;
    plain.reserve(value.size());

    bool in_tag = false;
    for (char c : value) {
        if (c == '<') {
            in_tag = true;
            continue;
        }
        if (c == '>') {
            in_tag = false;
            continue;
        }
        if (!in_tag) {
            plain += c;
        }
    }

    return plain;
}

std::string extractJsonString(const std::string& json, const std::string& key)
{
    const std::string marker = "\"" + key + "\":\"";
    std::size_t start = json.find(marker);
    if (start == std::string::npos) {
        return {};
    }

    start += marker.size();
    std::string out;
    out.reserve(256);

    bool escape = false;
    for (std::size_t i = start; i < json.size(); ++i) {
        const char ch = json[i];
        if (escape) {
            switch (ch) {
                case 'n': out += '\n'; break;
                case 't': out += '\t'; break;
                case 'r': out += '\r'; break;
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case '/': out += '/'; break;
                default: out += ch; break;
            }
            escape = false;
            continue;
        }

        if (ch == '\\') {
            escape = true;
            continue;
        }

        if (ch == '"') {
            break;
        }

        out += ch;
    }

    return out;
}

std::string runCommand(const std::string& command, int& exit_code)
{
    std::array<char, 512> buffer{};
    std::string output;

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        exit_code = -1;
        return {};
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        output += buffer.data();
    }

    exit_code = pclose(pipe);
    return output;
}

std::string decodeDuckDuckGoRedirect(std::string link)
{
    if (link.rfind("//", 0) == 0) {
        link = "https:" + link;
    }

    const std::size_t uddg_pos = link.find("uddg=");
    if (uddg_pos == std::string::npos) {
        return link;
    }

    std::string encoded = link.substr(uddg_pos + 5);
    const std::size_t amp_pos = encoded.find('&');
    if (amp_pos != std::string::npos) {
        encoded = encoded.substr(0, amp_pos);
    }

    return urlDecode(encoded);
}

std::string extractFirstHtmlResult(const std::string& html)
{
    std::size_t marker = html.find("result__a");
    if (marker == std::string::npos) {
        marker = html.find("result-link");
    }
    if (marker == std::string::npos) {
        return {};
    }

    std::size_t a_start = html.rfind("<a", marker);
    if (a_start == std::string::npos) {
        return {};
    }

    std::size_t href_start = html.find("href=\"", a_start);
    if (href_start == std::string::npos) {
        return {};
    }
    href_start += 6;

    std::size_t href_end = html.find('"', href_start);
    if (href_end == std::string::npos) {
        return {};
    }

    std::string link = html.substr(href_start, href_end - href_start);
    link = decodeDuckDuckGoRedirect(link);
    link = htmlDecode(link);
    link = trim(link);

    std::size_t title_start = html.find('>', href_end);
    if (title_start == std::string::npos) {
        return {};
    }
    ++title_start;

    std::size_t title_end = html.find("</a>", title_start);
    if (title_end == std::string::npos) {
        return {};
    }

    std::string title = html.substr(title_start, title_end - title_start);
    title = stripTags(title);
    title = htmlDecode(title);
    title = trim(title);

    if (title.empty()) {
        return {};
    }

    if (link.empty()) {
        return title;
    }

    return "Top result: " + title + "\nURL: " + link;
}

} // namespace

std::string WebSearchTool::name() const {
    return "web_search";
}

std::string WebSearchTool::description() const {
    return "Search the internet using DuckDuckGo.";
}

std::string WebSearchTool::execute(
    const std::map<std::string, std::string>& arguments)
{
    auto it = arguments.find("query");

    if (it == arguments.end()) {
        return "No search query provided.";
    }

    return performSearch(it->second);
}

std::string WebSearchTool::performSearch(const std::string& query)
{
    if (query.empty()) {
        return "No search query provided.";
    }

    const std::string encoded_query = urlEncode(query);
    const std::string fallback_url = "https://duckduckgo.com/?q=" + encoded_query;
    const std::string api_command =
        "/usr/bin/curl -sL --max-time 15 \"https://api.duckduckgo.com/?q=" +
        encoded_query +
        "&format=json&no_html=1\"";

    int exit_code = 0;
    const std::string api_result = runCommand(api_command, exit_code);

    const std::string heading = extractJsonString(api_result, "Heading");
    const std::string abstract = extractJsonString(api_result, "AbstractText");
    const std::string snippet = extractJsonString(api_result, "Text");
    const std::string first_url = extractJsonString(api_result, "FirstURL");

    if (!abstract.empty()) {
        if (!heading.empty()) {
            return heading + "\n" + abstract;
        }
        return abstract;
    }

    if (!snippet.empty()) {
        if (!first_url.empty()) {
            return snippet + "\nURL: " + first_url;
        }
        return snippet;
    }

    if (!heading.empty()) {
        if (!first_url.empty()) {
            return heading + "\nURL: " + first_url;
        }
        return heading;
    }

    const std::string html_command =
        "/usr/bin/curl -sL --max-time 15 \"https://html.duckduckgo.com/html/?q=" +
        encoded_query + "\"";
    const std::string html_result = runCommand(html_command, exit_code);

    const std::string top_result = extractFirstHtmlResult(html_result);
    if (!top_result.empty()) {
        return top_result;
    }

    if (exit_code != 0) {
        return "Web search unavailable. Open: " + fallback_url;
    }

    if (!api_result.empty()) {
        return "No instant summary found for this query.\nOpen: " + fallback_url;
    }

    return "No results found. Open: " + fallback_url;
}

} // namespace zipper
