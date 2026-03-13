#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace zipper::rag {

class TextChunker {
public:
    TextChunker(std::size_t chunk_size_tokens = 500, std::size_t overlap_tokens = 50);

    std::vector<std::string> chunk_text(const std::string& text) const;

private:
    std::size_t chunk_size_tokens_;
    std::size_t overlap_tokens_;
};

} // namespace zipper::rag
