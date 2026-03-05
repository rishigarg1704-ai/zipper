#pragma once

#include <string>
#include <vector>

#include "conversation_manager.hpp"

namespace zipper {

class PromptBuilder {
public:

    void set_system_prompt(const std::string& system);

    std::string build(const std::vector<Message>& history) const;

private:

    std::string system_prompt_ = "You are a helpful AI assistant.";
};

}