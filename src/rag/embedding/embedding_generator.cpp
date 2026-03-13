#include "embedding_generator.hpp"

namespace zipper::rag {

EmbeddingGenerator::EmbeddingGenerator(std::size_t dimension)
    : fallback_dimension_(dimension > 0 ? dimension : 384)
{
}

std::vector<float> EmbeddingGenerator::generate_embedding(const std::string& text) const
{
    return backend_.embed(text);
}

bool EmbeddingGenerator::ensure_loaded() const
{
    return backend_.ensure_loaded();
}

bool EmbeddingGenerator::ready() const
{
    return backend_.is_ready();
}

std::size_t EmbeddingGenerator::dimension() const
{
    const std::size_t backend_dimension = backend_.dimension();
    return backend_dimension > 0 ? backend_dimension : fallback_dimension_;
}

const std::string& EmbeddingGenerator::last_error() const
{
    return backend_.last_error();
}

void EmbeddingGenerator::unload()
{
    backend_.unload();
}

} // namespace zipper::rag
