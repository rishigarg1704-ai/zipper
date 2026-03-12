#pragma once

#include <map>
#include <string>

namespace zipper {

class ToolParser {
public:
    static bool parse(
        const std::string& input,
        std::string& toolName,
        std::map<std::string, std::string>& arguments
    );
};

} // namespace zipper
