#include "text_chunker.hpp"

#include <algorithm>
#include <sstream>

namespace zipper::rag {

TextChunker::TextChunker(std::size_t chunk_size_tokens, std::size_t overlap_tokens)
    : chunk_size_tokens_(chunk_size_tokens),
      overlap_tokens_(overlap_tokens)
{
    if (chunk_size_tokens_ == 0) {
        chunk_size_tokens_ = 500;
    }

    if (overlap_tokens_ >= chunk_size_tokens_) {
        overlap_tokens_ = chunk_size_tokens_ > 1 ? (chunk_size_tokens_ - 1) : 0;
    }
}

std::vector<std::string> TextChunker::chunk_text(const std::string& text) const
{
    std::vector<std::string> tokens;
    tokens.reserve(text.size() / 5);

    std::istringstream in(text);
    std::string token;
    while (in >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        return {};
    }

    const std::size_t step =
        chunk_size_tokens_ > overlap_tokens_ ? (chunk_size_tokens_ - overlap_tokens_) : 1;

    std::vector<std::string> chunks;
    for (std::size_t start = 0; start < tokens.size(); start += step) {
        const std::size_t end = std::min(start + chunk_size_tokens_, tokens.size());
        if (start >= end) {
            break;
        }

        std::ostringstream chunk;
        for (std::size_t i = start; i < end; ++i) {
            if (i > start) {
                chunk << ' ';
            }
            chunk << tokens[i];
        }

        chunks.push_back(chunk.str());

        if (end == tokens.size()) {
            break;
        }
    }

    return chunks;
}

} // namespace zipper::rag
