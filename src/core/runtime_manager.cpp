#include "runtime_manager.hpp"

namespace zipper {

RuntimeManager::RuntimeManager(const std::string& model_path,
                               int context_size,
                               int n_threads,
                               int max_tokens,
                               float temperature,
                               float top_p)
    : model_path_(model_path),
      context_size_(context_size),
      n_threads_(n_threads),
      max_tokens_(max_tokens),
      temperature_(temperature),
      top_p_(top_p) {
}

bool RuntimeManager::load_model() {
    return backend_.load_model(model_path_, context_size_, n_threads_);
}

void RuntimeManager::unload_model() {
    backend_.unload_model();
}

void RuntimeManager::set_system_prompt(const std::string& system) {
    prompt_builder_.set_system_prompt(system);
}

void RuntimeManager::generate(const std::string& user_input,
                               LlamaBackend::TokenCallback callback) {

    // Add user message to history
    conversation_.add_user_message(user_input);

    // Build prompt from conversation history
    std::string prompt = prompt_builder_.build(conversation_.get_history());

    // Collect assistant response
    std::string response;

    // Generate response
    backend_.generate(prompt, max_tokens_, temperature_, top_p_,
        [&](const std::string& token) {
            response += token;
            callback(token);
        });

    // Add assistant response to history
    conversation_.add_assistant_message(response);
}

void RuntimeManager::clear_conversation() {
    conversation_.clear();
}

}