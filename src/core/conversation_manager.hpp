#pragma once

#include <string>
#include <vector>

namespace zipper {

struct Message {
    std::string role;
    std::string content;
};

class ConversationManager {
public:

    void add_user_message(const std::string& text);

    void add_assistant_message(const std::string& text);

    const std::vector<Message>& get_history() const;

    void clear();

private:

    std::vector<Message> history_;
};

}