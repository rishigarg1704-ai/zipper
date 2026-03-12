#pragma once

#include <map>
#include <string>

namespace zipper {

class Tool {
public:
    using Arguments = std::map<std::string, std::string>;

    Tool() = default;
    virtual ~Tool() = default;

    Tool(const Tool&) = delete;
    Tool& operator=(const Tool&) = delete;
    Tool(Tool&&) = default;
    Tool& operator=(Tool&&) = default;

    [[nodiscard]] virtual std::string name() const = 0;
    [[nodiscard]] virtual std::string description() const = 0;

    [[nodiscard]] virtual std::string execute(
        const Arguments& arguments) = 0;
};

} // namespace zipper
