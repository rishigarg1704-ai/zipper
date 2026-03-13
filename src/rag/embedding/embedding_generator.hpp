#pragma once

#include "embedding_backend.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace zipper::rag {

class EmbeddingGenerator {
public:
    explicit EmbeddingGenerator(std::size_t dimension = 384);

    std::vector<float> generate_embedding(const std::string& text) const;
    bool ensure_loaded() const;
    bool ready() const;
    std::size_t dimension() const;
    const std::string& last_error() const;
    void unload();

private:
    std::size_t fallback_dimension_;
    mutable EmbeddingBackend backend_;
};

} // namespace zipper::rag
