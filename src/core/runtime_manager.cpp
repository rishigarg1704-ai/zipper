#include "runtime_manager.hpp"

#include "../tools/tool_parser.hpp"

#include <map>

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
      top_p_(top_p),
      context_manager_(context_size, max_tokens),
      tool_executor_(tool_registry_)
{
}

bool RuntimeManager::load_model() {
    return backend_.load_model(model_path_, context_size_, n_threads_);
}

void RuntimeManager::unload_model() {
    backend_.unload_model();
}

void RuntimeManager::set_system_prompt(const std::string& system) {

    prompt_builder_.set_system_prompt(system);
    context_manager_.set_system_prompt(system);
}

void RuntimeManager::generate(const std::string& user_input,
                              LlamaBackend::TokenCallback callback)
{
    conversation_.add_user_message(user_input);

    std::string tool_name;
    std::map<std::string, std::string> arguments;

    if (ToolParser::parse(user_input, tool_name, arguments)) {

        std::string result = tool_executor_.execute(tool_name, arguments);

        callback(result);
        conversation_.add_assistant_message(result);

        return;
    }

    auto trimmed = context_manager_.prepare_context(
        conversation_.get_history()
    );

    std::string prompt = prompt_builder_.build(trimmed);

    std::string response;

    backend_.generate(prompt, max_tokens_, temperature_, top_p_,
        [&](const std::string& token) {

            response += token;
            callback(token);
        });

    conversation_.add_assistant_message(response);
}

void RuntimeManager::clear_conversation() {

    conversation_.clear();
    backend_.clear_kv_cache();
}

} // namespace zipper
