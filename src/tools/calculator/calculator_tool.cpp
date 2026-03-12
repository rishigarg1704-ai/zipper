#include "calculator_tool.hpp"

#include <sstream>

namespace zipper {

std::string CalculatorTool::name() const {
    return "calculator";
}

std::string CalculatorTool::description() const {
    return "Performs simple arithmetic calculations.";
}

std::string CalculatorTool::execute(
        const std::map<std::string, std::string>& arguments)
{
    auto it = arguments.find("expression");

    if (it == arguments.end())
        return "No expression provided.";

    std::string expr = it->second;

    std::stringstream ss(expr);

    double a, b;
    char op;

    ss >> a >> op >> b;

    double result = 0;

    switch (op) {

        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/': result = b != 0 ? a / b : 0; break;

        default:
            return "Unsupported operation.";
    }

    return std::to_string(result);
}

}