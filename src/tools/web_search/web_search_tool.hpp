// src/tools/web_search/web_search_tool.hpp
#pragma once

#include "../tool.hpp"
#include <map>
#include <string>

namespace zipper {

class WebSearchTool : public Tool {
public:

    WebSearchTool() = default;
    ~WebSearchTool() override = default;

    std::string name() const override;

    std::string description() const override;

    std::string execute(
        const std::map<std::string, std::string>& arguments
    ) override;

private:

    std::string performSearch(const std::string& query);

};

} // namespace zipper
