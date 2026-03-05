#pragma once

#include <string>
#include <vector>

namespace zipper {

class SessionManager {
public:
    void add_user(const std::string& message);
    void add_assistant(const std::string& message);
    void reset();

    std::string build_prompt() const;

private:
    struct Message {
        std::string role;
        std::string content;
    };

    std::vector<Message> messages_;
};

}