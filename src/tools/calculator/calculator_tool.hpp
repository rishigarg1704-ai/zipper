#pragma once

#include "../tool.hpp"

namespace zipper {

class CalculatorTool : public Tool {
public:

    std::string name() const override;

    std::string description() const override;

    std::string execute(
        const std::map<std::string, std::string>& arguments
    ) override;

};

}