#include "tool_registry.hpp"

namespace zipper {

void ToolRegistry::register_tool(std::shared_ptr<Tool> tool) {
    tools_[tool->name()] = std::move(tool);
}

Tool* ToolRegistry::get_tool(const std::string& name) {
    auto it = tools_.find(name);
    if (it == tools_.end()) {
        return nullptr;
    }
    return it->second.get();
}

} // namespace zipper
