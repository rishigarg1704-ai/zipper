#include "prompt_builder.hpp"

namespace zipper {

void PromptBuilder::set_system_prompt(const std::string& system) {
    system_prompt_ = system;
}

std::string PromptBuilder::build(const std::vector<Message>& history) const {

    std::string prompt;

    // Add system prompt
    prompt += "System: " + system_prompt_ + "\n\n";

    // Add conversation history
    for (const auto& msg : history) {
        if (msg.role == "user") {
            prompt += "User: " + msg.content + "\n";
        } else if (msg.role == "assistant") {
            prompt += "Assistant: " + msg.content + "\n";
        }
    }

    // Add final prompt for assistant to respond
    prompt += "Assistant:";

    return prompt;
}

}