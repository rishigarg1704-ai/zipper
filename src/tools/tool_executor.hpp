#pragma once

#include <string>
#include <map>

#include "tool_registry.hpp"

namespace zipper {

class ToolExecutor {
public:

    ToolExecutor(ToolRegistry& registry);

    std::string execute(
        const std::string& tool_name,
        const std::map<std::string, std::string>& arguments
    );

private:

    ToolRegistry& registry_;
};

}