#include "context_builder.hpp"

#include <cctype>
#include <sstream>

namespace zipper::rag {
namespace {

bool is_word_char(unsigned char c)
{
    return std::isalnum(c) != 0;
}

std::size_t estimate_token_count(const std::string& text)
{
    std::size_t token_count = 0;
    bool in_word = false;

    for (unsigned char c : text) {
        if (is_word_char(c)) {
            if (!in_word) {
                ++token_count;
                in_word = true;
            }
            continue;
        }

        in_word = false;
        if (!std::isspace(c)) {
            ++token_count;
        }
    }

    return token_count;
}

std::string truncate_to_token_limit(const std::string& text, std::size_t max_tokens)
{
    if (max_tokens == 0U || text.empty()) {
        return {};
    }

    std::size_t token_count = 0;
    bool in_word = false;
    std::size_t cut_position = text.size();

    for (std::size_t i = 0; i < text.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(text[i]);

        if (is_word_char(c)) {
            if (!in_word) {
                ++token_count;
                if (token_count > max_tokens) {
                    cut_position = i;
                    break;
                }
                in_word = true;
            }
            continue;
        }

        in_word = false;
        if (!std::isspace(c)) {
            ++token_count;
            if (token_count > max_tokens) {
                cut_position = i;
                break;
            }
        }
    }

    std::string truncated = text.substr(0, cut_position);
    while (!truncated.empty() &&
           std::isspace(static_cast<unsigned char>(truncated.back()))) {
        truncated.pop_back();
    }

    return truncated;
}

} // namespace

ContextBuilder::ContextBuilder(std::size_t max_context_tokens)
    : max_context_tokens_(max_context_tokens > 0 ? max_context_tokens : 1200)
{
}

std::string ContextBuilder::build_context_prompt(
    const std::vector<SearchResult>& retrieved_chunks,
    const std::string& question) const
{
    std::ostringstream prompt;

    prompt << "Use the following context to answer the question.\n";
    prompt << "If the answer is not in the context, say you do not know.\n\n";
    prompt << "Context:\n";

    std::size_t used_context_tokens = 0;

    for (const auto& chunk : retrieved_chunks) {
        const std::string doc_label =
            chunk.source_file.empty() ? chunk.document_id : chunk.source_file;

        const std::string header =
            "[doc: " + doc_label + " | chunk: " + std::to_string(chunk.chunk_id) + "]\n";
        const std::size_t header_tokens = estimate_token_count(header);

        if (used_context_tokens >= max_context_tokens_ ||
            header_tokens >= max_context_tokens_ - used_context_tokens) {
            break;
        }

        const std::size_t remaining_tokens =
            max_context_tokens_ - used_context_tokens - header_tokens;
        const std::string bounded_chunk =
            truncate_to_token_limit(chunk.chunk_text, remaining_tokens);

        if (bounded_chunk.empty()) {
            continue;
        }

        prompt << header;
        prompt << bounded_chunk << "\n\n";

        used_context_tokens += header_tokens + estimate_token_count(bounded_chunk);
    }

    prompt << "Question:\n" << question;
    return prompt.str();
}

} // namespace zipper::rag
