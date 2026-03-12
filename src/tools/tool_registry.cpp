// src/tools/tool_registry.cpp
#include "tool_registry.hpp"

#include "calculator/calculator_tool.hpp"
#include "date/date_tool.hpp"
#include "file_reader/file_reader_tool.hpp"
#include "stock/stock_tool.hpp"
#include "system_info/system_info_tool.hpp"
#include "web_search/web_search_tool.hpp"

namespace zipper {

ToolRegistry::ToolRegistry() {

    registerTool(std::make_unique<CalculatorTool>());
    registerTool(std::make_unique<DateTool>());
    registerTool(std::make_unique<SystemInfoTool>());
    registerTool(std::make_unique<FileReaderTool>());
    registerTool(std::make_unique<StockTool>());
    registerTool(std::make_unique<WebSearchTool>());

}

void ToolRegistry::registerTool(std::unique_ptr<Tool> tool) {

    std::string toolName = tool->name();
    tools_[toolName] = std::move(tool);

}

Tool* ToolRegistry::getTool(const std::string& name) {

    auto it = tools_.find(name);

    if (it == tools_.end())
        return nullptr;

    return it->second.get();
}

const std::map<std::string, std::unique_ptr<Tool>>&
ToolRegistry::getTools() const {

    return tools_;
}

} // namespace zipper
