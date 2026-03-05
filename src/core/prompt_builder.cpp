#include "prompt_builder.hpp"

namespace zipper {

void PromptBuilder::set_system_prompt(const std::string& system) {
    system_prompt_ = system;
}

std::string PromptBuilder::build(const std::vector<Message>& context) const {

    std::string prompt;

    // Build prompt from context (system prompt already included)
    for (const auto& msg : context) {
        if (msg.role == "system") {
            prompt += "System: " + msg.content + "\n\n";
        } else if (msg.role == "user") {
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