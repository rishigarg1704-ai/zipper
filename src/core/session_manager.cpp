#include "core/session_manager.hpp"

namespace zipper {

void SessionManager::add_user(const std::string& message) {
    messages_.push_back({"user", message});
}

void SessionManager::add_assistant(const std::string& message) {
    messages_.push_back({"assistant", message});
}

void SessionManager::reset() {
    messages_.clear();
}

std::string SessionManager::build_prompt() const {

    std::string result;

    for (const auto& msg : messages_) {
        result += msg.role + ": " + msg.content + "\n";
    }

    result += "assistant: ";
    return result;
}

}