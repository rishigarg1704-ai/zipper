#include "runtime_manager.hpp"

#include "../tools/tool_parser.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <sstream>
#include <string>

namespace zipper {
namespace {

std::string to_lower_copy(std::string value)
{
    std::transform(
        value.begin(),
        value.end(),
        value.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string trim_copy(const std::string& value)
{
    std::size_t start = 0;
    while (start < value.size() && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

bool starts_with(const std::string& text, const std::string& prefix)
{
    return text.size() >= prefix.size() &&
           std::equal(prefix.begin(), prefix.end(), text.begin());
}

} // namespace

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
      tool_executor_(tool_registry_),
      rag_retriever_(rag_embedding_generator_, rag_vector_store_)
{
}

bool RuntimeManager::load_model() {
    return backend_.load_model(model_path_, context_size_, n_threads_);
}

void RuntimeManager::unload_model() {
    rag_embedding_generator_.unload();
    backend_.unload_model();
}

void RuntimeManager::set_system_prompt(const std::string& system) {

    prompt_builder_.set_system_prompt(system);
    context_manager_.set_system_prompt(system);
}

void RuntimeManager::generate(const std::string& user_input,
                              LlamaBackend::TokenCallback callback)
{
    std::string tool_name;
    std::map<std::string, std::string> arguments;

    if (ToolParser::parse(user_input, tool_name, arguments)) {
        conversation_.add_user_message(user_input);

        std::string result = tool_executor_.execute(tool_name, arguments);

        callback(result);
        conversation_.add_assistant_message(result);

        return;
    }

    std::string rag_response;
    if (handle_rag_command(user_input, rag_response)) {
        callback(rag_response);
        return;
    }

    conversation_.add_user_message(user_input);

    auto trimmed = context_manager_.prepare_context(
        conversation_.get_history()
    );

    std::vector<rag::SearchResult> retrieved_chunks;

    if (!rag_vector_store_.empty()) {
        retrieved_chunks = rag_retriever_.retrieve(user_input, 5);
        if (!retrieved_chunks.empty()) {
            const std::string rag_user_message =
                rag_context_builder_.build_context_prompt(retrieved_chunks, user_input);

            for (auto it = trimmed.rbegin(); it != trimmed.rend(); ++it) {
                if (it->role == "user") {
                    it->content = rag_user_message;
                    break;
                }
            }
        }
    }

    std::string prompt = prompt_builder_.build(trimmed);

    std::string response;

    const bool generation_succeeded =
        backend_.generate(prompt, max_tokens_, temperature_, top_p_,
            [&](const std::string& token) {

                response += token;
                callback(token);
            });

    if (!generation_succeeded) {
        return;
    }

    if (!retrieved_chunks.empty()) {
        std::ostringstream sources;
        sources << "\n\nSources:\n";

        std::set<std::string> seen;
        for (const auto& chunk : retrieved_chunks) {
            const std::string doc_label =
                chunk.source_file.empty() ? chunk.document_id : chunk.source_file;
            const std::string citation =
                "[doc: " + doc_label + " | chunk: " + std::to_string(chunk.chunk_id) + "]";

            if (seen.insert(citation).second) {
                sources << "- " << citation << '\n';
            }
        }

        const std::string sources_text = sources.str();
        response += sources_text;
        callback(sources_text);
    }

    conversation_.add_assistant_message(response);
}

bool RuntimeManager::handle_rag_command(const std::string& user_input, std::string& response)
{
    const std::string trimmed_input = trim_copy(user_input);
    const std::string lowered = to_lower_copy(trimmed_input);

    if (lowered == "rag status") {
        response = "RAG status:\nDocuments: " + std::to_string(rag_vector_store_.document_count()) +
                   "\nChunks: " + std::to_string(rag_vector_store_.chunk_count());
        return true;
    }

    if (lowered == "rag clear") {
        rag_vector_store_.clear();
        response = "RAG index cleared.";
        return true;
    }

    const std::string prefix_1 = "load document ";
    const std::string prefix_2 = "load doc ";
    const std::string prefix_3 = "rag load ";

    std::string path;
    if (starts_with(lowered, prefix_1)) {
        path = trim_copy(trimmed_input.substr(prefix_1.size()));
    } else if (starts_with(lowered, prefix_2)) {
        path = trim_copy(trimmed_input.substr(prefix_2.size()));
    } else if (starts_with(lowered, prefix_3)) {
        path = trim_copy(trimmed_input.substr(prefix_3.size()));
    } else {
        return false;
    }

    if (path.empty()) {
        response = "Usage: load document <path_to_txt_or_md>";
        return true;
    }

    const rag::DocumentLoadResult loaded = rag_document_loader_.load_document(path);
    if (!loaded.success) {
        response = "RAG load failed: " + loaded.error;
        return true;
    }

    const auto chunks = rag_text_chunker_.chunk_text(loaded.text);
    if (chunks.empty()) {
        response = "RAG load failed: no text chunks extracted.";
        return true;
    }

    if (!rag_embedding_generator_.ensure_loaded()) {
        response = "RAG load failed: " + rag_embedding_generator_.last_error();
        return true;
    }

    rag_vector_store_.add_document(loaded.resolved_path, chunks, rag_embedding_generator_);

    response = "RAG indexed document: " + loaded.resolved_path +
               "\nChunks indexed: " + std::to_string(chunks.size()) +
               "\nTotal documents: " + std::to_string(rag_vector_store_.document_count()) +
               "\nTotal chunks: " + std::to_string(rag_vector_store_.chunk_count());

    return true;
}

void RuntimeManager::clear_conversation() {

    conversation_.clear();
    backend_.clear_kv_cache();
}

} // namespace zipper
