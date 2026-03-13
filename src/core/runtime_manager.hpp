#pragma once

#include <string>
#include <functional>

#include "../backend/llama_backend.hpp"
#include "conversation_manager.hpp"
#include "prompt_builder.hpp"
#include "context_manager.hpp"

#include "../tools/tool_registry.hpp"
#include "../tools/tool_executor.hpp"

#include "../rag/document_loader/document_loader.hpp"
#include "../rag/text_chunker/text_chunker.hpp"
#include "../rag/embedding/embedding_generator.hpp"
#include "../rag/vector_store/vector_store.hpp"
#include "../rag/retriever/retriever.hpp"
#include "../rag/context_builder/context_builder.hpp"

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
    bool handle_rag_command(const std::string& user_input, std::string& response);

    std::string model_path_;
    int context_size_;
    int n_threads_;
    int max_tokens_;
    float temperature_;
    float top_p_;

    LlamaBackend backend_;
    ConversationManager conversation_;
    PromptBuilder prompt_builder_;
    ContextManager context_manager_;

    ToolRegistry tool_registry_;
    ToolExecutor tool_executor_;

    rag::DocumentLoader rag_document_loader_;
    rag::TextChunker rag_text_chunker_;
    rag::EmbeddingGenerator rag_embedding_generator_;
    rag::VectorStore rag_vector_store_;
    rag::Retriever rag_retriever_;
    rag::ContextBuilder rag_context_builder_;
};

}
