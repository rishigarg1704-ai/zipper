#include "tool_executor.hpp"

namespace zipper {

ToolExecutor::ToolExecutor(ToolRegistry& registry)
    : registry_(registry) {}

std::string ToolExecutor::execute(
    const std::string& tool_name,
    const std::map<std::string, std::string>& arguments) {
    Tool* tool = registry_.get_tool(tool_name);
    if (!tool) {
        return "Tool not found.";
    }
    return tool->execute(arguments);
}

} // namespace zipper
