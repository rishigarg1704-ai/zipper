#pragma once

#include <map>
#include <string>

#include "tool_registry.hpp"

namespace zipper {

class ToolExecutor {
public:
    explicit ToolExecutor(ToolRegistry& registry);

    std::string execute(
        const std::string& tool_name,
        const std::map<std::string, std::string>& arguments);

private:
    ToolRegistry& registry_;
};

} // namespace zipper
