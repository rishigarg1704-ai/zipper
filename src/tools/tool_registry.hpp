// src/tools/tool_registry.hpp
#pragma once

#include <map>
#include <memory>
#include <string>

#include "tool.hpp"

namespace zipper {

class ToolRegistry {
public:
    ToolRegistry();

    void registerTool(std::unique_ptr<Tool> tool);

    Tool* getTool(const std::string& name);

    const std::map<std::string, std::unique_ptr<Tool>>& getTools() const;

private:
    std::map<std::string, std::unique_ptr<Tool>> tools_;
};

} // namespace zipper
