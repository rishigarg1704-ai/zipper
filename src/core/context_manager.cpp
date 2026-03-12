// /Users/rishi/Desktop/zipper/src/core/context_manager.cpp
#include "context_manager.hpp"

namespace zipper {

ContextManager::ContextManager(int max_context_tokens,
                               int reserved_response_tokens)
    : max_context_tokens_(max_context_tokens),
      reserved_response_tokens_(reserved_response_tokens) {}

void ContextManager::set_system_prompt(const std::string& prompt) {
    system_prompt_ = prompt;
}

int ContextManager::estimate_tokens(const std::string& text) const {
    return text.size() / 4;
}

int ContextManager::estimate_message_tokens(const Message& msg) const {
    return estimate_tokens(msg.role) + estimate_tokens(msg.content);
}

int ContextManager::estimate_total_tokens(const std::vector<Message>& messages) const {
    int total = 0;
    for (const auto& m : messages) {
        total += estimate_message_tokens(m);
    }
    return total;
}

int ContextManager::get_max_prompt_tokens() const {
    return max_context_tokens_ - reserved_response_tokens_;
}

std::vector<Message> ContextManager::prepare_context(
    const std::vector<Message>& history) {

    std::vector<Message> result;

    int max_tokens = get_max_prompt_tokens();
    int used_tokens = 0;

    if (!system_prompt_.empty()) {
        Message sys;
        sys.role = "system";
        sys.content = system_prompt_;
        result.push_back(sys);
        used_tokens += estimate_message_tokens(sys);
    }

    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        int tokens = estimate_message_tokens(*it);

        if (used_tokens + tokens > max_tokens) {
            break;
        }

        result.insert(result.begin() + (system_prompt_.empty() ? 0 : 1), *it);
        used_tokens += tokens;
    }

    return result;
}

} // namespace zipper
