#include "retriever.hpp"

#include <algorithm>
#include <cstdlib>
#include <string>

namespace zipper::rag {
namespace {

float read_min_similarity_from_env()
{
    constexpr float kDefaultThreshold = 0.25f;
    const char* env_value = std::getenv("ZIPPER_RAG_MIN_SIMILARITY");
    if (!env_value || *env_value == '\0') {
        return kDefaultThreshold;
    }

    try {
        float threshold = std::stof(std::string(env_value));
        threshold = std::max(-1.0f, std::min(1.0f, threshold));
        return threshold;
    } catch (...) {
        return kDefaultThreshold;
    }
}

} // namespace

Retriever::Retriever(const EmbeddingGenerator& embedding_generator,
                     const VectorStore& vector_store)
    : min_similarity_(read_min_similarity_from_env()),
      embedding_generator_(embedding_generator),
      vector_store_(vector_store)
{
}

std::vector<SearchResult> Retriever::retrieve(const std::string& query,
                                              std::size_t top_k) const
{
    if (query.empty()) {
        return {};
    }

    const std::vector<float> query_embedding =
        embedding_generator_.generate_embedding(query);

    std::vector<SearchResult> results = vector_store_.search(query_embedding, top_k);

    results.erase(
        std::remove_if(
            results.begin(),
            results.end(),
            [&](const SearchResult& result) {
                return result.score < min_similarity_;
            }),
        results.end());

    return results;
}

} // namespace zipper::rag
