#pragma once

#include "../vector_store/vector_store.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace zipper::rag {

class ContextBuilder {
public:
    explicit ContextBuilder(std::size_t max_context_tokens = 1200);

    std::string build_context_prompt(const std::vector<SearchResult>& retrieved_chunks,
                                     const std::string& question) const;

private:
    std::size_t max_context_tokens_ = 1200;
};

} // namespace zipper::rag
