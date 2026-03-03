#include "session_manager.hpp"
#include "../../external/json/json.hpp"

#include <fstream>
#include <sstream>

using json = nlohmann::json;

namespace zipper {

SessionManager::SessionManager() {
    load();
}

void SessionManager::load() {
    std::ifstream file(SESSION_FILE);

    if (!file.is_open()) {
        system_prompt_ = DEFAULT_SYSTEM_PROMPT;
        return;
    }

    json j;
    file >> j;

    system_prompt_ = j.value("system_prompt", DEFAULT_SYSTEM_PROMPT);

    if (j.contains("messages")) {
        for (auto& msg : j["messages"]) {
            messages_.push_back({
                msg["role"],
                msg["content"],
                msg["timestamp"]
            });
        }
    }
}

void SessionManager::save() {
    json j;
    j["system_prompt"] = system_prompt_;
    j["messages"] = json::array();

    for (auto& msg : messages_) {
        j["messages"].push_back({
            {"role", msg.role},
            {"content", msg.content},
            {"timestamp", msg.timestamp}
        });
    }

    std::ofstream file(SESSION_FILE);
    file << j.dump(4);
}

void SessionManager::add_user_message(const std::string& content) {
    messages_.push_back({"user", content, std::time(nullptr)});
}

void SessionManager::add_assistant_message(const std::string& content) {
    messages_.push_back({"assistant", content, std::time(nullptr)});
    smart_trim();
    save();
}

void SessionManager::reset() {
    messages_.clear();
    system_prompt_ = DEFAULT_SYSTEM_PROMPT;
    save();
}

std::string SessionManager::build_prompt() {
    std::stringstream ss;

    ss << "<|begin_of_text|>\n";
    ss << "<|start_header_id|>system<|end_header_id|>\n";
    ss << system_prompt_ << "\n";
    ss << "<|eot_id|>\n";

    for (auto& msg : messages_) {
        ss << "<|start_header_id|>" << msg.role << "<|end_header_id|>\n";
        ss << msg.content << "\n";
        ss << "<|eot_id|>\n";
    }

    ss << "<|start_header_id|>assistant<|end_header_id|>\n";

    return ss.str();
}

int SessionManager::estimate_tokens(const std::string& text) {
    return text.length() / 4; // rough approximation
}

void SessionManager::smart_trim() {

    while (true) {
        std::string prompt = build_prompt();
        int estimated = estimate_tokens(prompt);

        if (estimated <= TOKEN_LIMIT) break;

        if (messages_.size() >= 2) {
            messages_.erase(messages_.begin());
            messages_.erase(messages_.begin());
        } else {
            break;
        }
    }
}

}