#pragma once

#include <map>
#include <memory>
#include <string>

#include "tool.hpp"

namespace zipper {

class ToolRegistry {
public:
    void register_tool(std::shared_ptr<Tool> tool);
    Tool* get_tool(const std::string& name);

private:
    std::map<std::string, std::shared_ptr<Tool>> tools_;
};

} // namespace zipper
