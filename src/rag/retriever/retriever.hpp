#pragma once

#include "../embedding/embedding_generator.hpp"
#include "../vector_store/vector_store.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace zipper::rag {

class Retriever {
public:
    Retriever(const EmbeddingGenerator& embedding_generator,
              const VectorStore& vector_store);

    std::vector<SearchResult> retrieve(const std::string& query,
                                       std::size_t top_k = 5) const;

private:
    float min_similarity_ = 0.25f;

    const EmbeddingGenerator& embedding_generator_;
    const VectorStore& vector_store_;
};

} // namespace zipper::rag
