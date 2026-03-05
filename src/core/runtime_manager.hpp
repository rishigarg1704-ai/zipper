#pragma once

#include <string>
#include <functional>

#include "../backend/llama_backend.hpp"
#include "conversation_manager.hpp"
#include "prompt_builder.hpp"

namespace zipper {

class RuntimeManager {
public:

    RuntimeManager(const std::string& model_path,
                   int context_size,
                   int n_threads,
                   int max_tokens,
                   float temperature,
                   float top_p);

    ~RuntimeManager() = default;

    bool load_model();

    void unload_model();

    void set_system_prompt(const std::string& system);

    void generate(const std::string& user_input,
                  LlamaBackend::TokenCallback callback);

    void clear_conversation();

private:
    std::string model_path_;
    int context_size_;
    int n_threads_;
    int max_tokens_;
    float temperature_;
    float top_p_;

    LlamaBackend backend_;
    ConversationManager conversation_;
    PromptBuilder prompt_builder_;
};

}