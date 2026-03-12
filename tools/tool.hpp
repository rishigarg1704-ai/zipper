#pragma once

#include <map>
#include <string>

namespace zipper {

class Tool {
public:
    virtual ~Tool() = default;

    virtual std::string name() const = 0;
    virtual std::string description() const = 0;

    virtual std::string execute(
        const std::map<std::string, std::string>& arguments) = 0;
};

} // namespace zipper
