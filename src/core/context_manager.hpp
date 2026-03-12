// /Users/rishi/Desktop/zipper/src/core/context_manager.hpp
#pragma once

#include <string>
#include <vector>

#include "conversation_manager.hpp"

namespace zipper {

class ContextManager {
public:
    ContextManager(int max_context_tokens = 4096,
                   int reserved_response_tokens = 512);

    void set_system_prompt(const std::string& prompt);

    std::vector<Message> prepare_context(
        const std::vector<Message>& history);

    int get_max_prompt_tokens() const;

private:
    int max_context_tokens_;
    int reserved_response_tokens_;
    std::string system_prompt_;

    int estimate_tokens(const std::string& text) const;
    int estimate_message_tokens(const Message& msg) const;
    int estimate_total_tokens(const std::vector<Message>& messages) const;
};

} // namespace zipper
