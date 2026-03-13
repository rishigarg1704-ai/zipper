#pragma once

#include "../embedding/embedding_generator.hpp"

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

namespace zipper::rag {

struct SearchResult {
    std::string document_id;
    std::size_t chunk_id = 0;
    std::string source_file;
    std::string chunk_text;
    float score = 0.0f;
};

class VectorStore {
public:
    VectorStore();

    void clear();

    void add_document(const std::string& document_id,
                      const std::vector<std::string>& chunks,
                      const EmbeddingGenerator& embedding_generator);

    std::vector<SearchResult> search(const std::vector<float>& query_embedding,
                                     std::size_t k = 5) const;

    bool empty() const;
    std::size_t document_count() const;
    std::size_t chunk_count() const;

private:
    struct VectorRecord {
        std::string document_id;
        std::size_t chunk_id = 0;
        std::string source_file;
        std::string chunk_text;
        std::vector<float> embedding;
    };

    static float cosine_similarity(const std::vector<float>& a, const std::vector<float>& b);
    void load_from_disk();
    void persist_to_disk() const;
    void remove_document_if_exists(const std::string& document_id);

    std::vector<VectorRecord> records_;
    std::vector<std::string> document_ids_;
};

} // namespace zipper::rag
