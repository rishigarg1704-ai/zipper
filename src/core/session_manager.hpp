#pragma once

#include <string>
#include <vector>
#include <ctime>

namespace zipper {

struct Message {
    std::string role;
    std::string content;
    std::time_t timestamp;
};

class SessionManager {
public:
    SessionManager();

    void load();
    void save();

    void add_user_message(const std::string& content);
    void add_assistant_message(const std::string& content);

    void reset();

    std::string build_prompt();

private:
    void smart_trim();
    int estimate_tokens(const std::string& text);

private:
    std::string system_prompt_;
    std::vector<Message> messages_;

    const std::string DEFAULT_SYSTEM_PROMPT =
        "You are Zipper, a high-performance local AI runtime built in C++. "
        "You run fully offline and prioritize clarity, correctness, and structured thinking.";

    const std::string SESSION_FILE = "config/session.json";
    const int TOKEN_LIMIT = 3500;
};

}