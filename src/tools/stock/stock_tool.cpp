#include "stock_tool.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdio>
#include <iomanip>
#include <sstream>

namespace zipper {
namespace {

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

std::string extractFirstResultObject(const std::string& json)
{
    const std::size_t result_pos = json.find("\"result\":[");
    if (result_pos == std::string::npos) {
        return {};
    }

    const std::size_t first_object = json.find('{', result_pos);
    if (first_object == std::string::npos) {
        return {};
    }

    int depth = 0;
    for (std::size_t i = first_object; i < json.size(); ++i) {
        if (json[i] == '{') {
            ++depth;
        } else if (json[i] == '}') {
            --depth;
            if (depth == 0) {
                return json.substr(first_object, i - first_object + 1);
            }
        }
    }

    return {};
}

std::string extractJsonField(const std::string& json, const std::string& key)
{
    const std::string marker = "\"" + key + "\":";
    std::size_t pos = json.find(marker);
    if (pos == std::string::npos) {
        return {};
    }

    pos += marker.size();
    while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) {
        ++pos;
    }
    if (pos >= json.size()) {
        return {};
    }

    if (json[pos] == '"') {
        ++pos;
        std::string value;
        bool escaped = false;

        for (; pos < json.size(); ++pos) {
            char c = json[pos];
            if (escaped) {
                switch (c) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '"': value += '"'; break;
                    case '\\': value += '\\'; break;
                    default: value += c; break;
                }
                escaped = false;
                continue;
            }

            if (c == '\\') {
                escaped = true;
                continue;
            }

            if (c == '"') {
                break;
            }

            value += c;
        }

        return trim(value);
    }

    std::size_t end = pos;
    while (end < json.size() && json[end] != ',' && json[end] != '}') {
        ++end;
    }

    return trim(json.substr(pos, end - pos));
}

std::string formatSignedPercent(const std::string& raw_percent)
{
    if (raw_percent.empty()) {
        return {};
    }

    try {
        const double value = std::stod(raw_percent);
        std::ostringstream out;
        out << std::fixed << std::setprecision(2);
        if (value >= 0.0) {
            out << '+';
        }
        out << value << '%';
        return out.str();
    } catch (...) {
        return raw_percent + '%';
    }
}

} // namespace

std::string StockTool::name() const
{
    return "stock";
}

std::string StockTool::description() const
{
    return "Fetches live stock quote data.";
}

std::string StockTool::execute(const std::map<std::string, std::string>& arguments)
{
    std::string query;

    const auto symbol_it = arguments.find("symbol");
    if (symbol_it != arguments.end()) {
        query = symbol_it->second;
    } else {
        const auto query_it = arguments.find("query");
        if (query_it != arguments.end()) {
            query = query_it->second;
        }
    }

    query = trim(query);
    if (query.empty()) {
        return "No stock query provided. Example: stock TSLA";
    }

    const std::string symbol = resolveSymbol(query);
    if (symbol.empty()) {
        return "Invalid ticker symbol. Use uppercase letters and digits only, e.g. /stock AAPL";
    }

    return fetchQuote(symbol);
}

std::string StockTool::resolveSymbol(const std::string& query) const
{
    const std::string symbol = trim(query);
    return isValidSymbol(symbol) ? symbol : std::string{};
}

bool StockTool::isValidSymbol(const std::string& symbol) const
{
    if (symbol.empty()) {
        return false;
    }

    return std::all_of(
        symbol.begin(),
        symbol.end(),
        [](unsigned char c) {
            return std::isdigit(c) != 0 || (std::isupper(c) != 0 && std::isalpha(c) != 0);
        });
}

std::string StockTool::fetchQuote(const std::string& symbol) const
{
    const std::string command =
        "/usr/bin/curl -sL --max-time 15 "
        "\"https://query1.finance.yahoo.com/v7/finance/quote?symbols=" + symbol + "\"";

    int exit_code = 0;
    const std::string json = runCommand(command, exit_code);
    if (exit_code != 0 || json.empty()) {
        return "Stock lookup failed for " + symbol + ".";
    }

    const std::string object = extractFirstResultObject(json);
    if (object.empty()) {
        return "No stock data found for " + symbol + ".";
    }

    const std::string symbol_value = extractJsonField(object, "symbol");
    const std::string short_name = extractJsonField(object, "shortName");
    const std::string price = extractJsonField(object, "regularMarketPrice");
    const std::string currency = extractJsonField(object, "currency");
    const std::string change_percent = extractJsonField(object, "regularMarketChangePercent");
    const std::string market_state = extractJsonField(object, "marketState");

    const std::string resolved_symbol = symbol_value.empty() ? symbol : symbol_value;

    if (price.empty()) {
        return "No price available for " + resolved_symbol +
               ".\nURL: https://finance.yahoo.com/quote/" + resolved_symbol;
    }

    std::ostringstream out;
    out << resolved_symbol;
    if (!short_name.empty()) {
        out << " (" << short_name << ")";
    }

    out << "\nPrice: ";
    if (currency == "USD" || currency.empty()) {
        out << '$';
    }
    out << price;
    if (!currency.empty() && currency != "USD") {
        out << ' ' << currency;
    }

    const std::string formatted_percent = formatSignedPercent(change_percent);
    if (!formatted_percent.empty()) {
        out << "\nChange: " << formatted_percent;
    }

    if (!market_state.empty()) {
        out << "\nMarket: " << market_state;
    }

    out << "\nURL: https://finance.yahoo.com/quote/" << resolved_symbol;
    return out.str();
}

} // namespace zipper
