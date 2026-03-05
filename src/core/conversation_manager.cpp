#include "conversation_manager.hpp"

namespace zipper {

void ConversationManager::add_user_message(const std::string& text) {
    history_.push_back({"user", text});
}

void ConversationManager::add_assistant_message(const std::string& text) {
    history_.push_back({"assistant", text});
}

const std::vector<Message>& ConversationManager::get_history() const {
    return history_;
}

void ConversationManager::clear() {
    history_.clear();
}

}