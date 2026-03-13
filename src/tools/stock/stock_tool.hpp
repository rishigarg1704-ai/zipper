#pragma once

#include "../tool.hpp"

namespace zipper {

class StockTool : public Tool {
public:
    std::string name() const override;
    std::string description() const override;
    std::string execute(const std::map<std::string, std::string>& arguments) override;

private:
    std::string resolveSymbol(const std::string& query) const;
    std::string fetchQuote(const std::string& symbol) const;
    bool isValidSymbol(const std::string& symbol) const;
};

} // namespace zipper
