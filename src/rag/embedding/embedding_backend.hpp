#pragma once

#include <cstddef>
#include <string>
#include <vector>

struct llama_model;
struct llama_context;

namespace zipper::rag {

class EmbeddingBackend {
public:
    EmbeddingBackend();
    ~EmbeddingBackend();

    EmbeddingBackend(const EmbeddingBackend&) = delete;
    EmbeddingBackend& operator=(const EmbeddingBackend&) = delete;

    std::vector<float> embed(const std::string& text) const;

    bool ensure_loaded() const;
    bool is_ready() const;
    std::size_t dimension() const;
    const std::string& model_path() const;
    const std::string& last_error() const;
    void unload();

private:
    bool load_model() const;
    std::vector<float> zero_vector() const;
    static void normalize(std::vector<float>& values);

    mutable llama_model* model_ = nullptr;
    mutable llama_context* ctx_ = nullptr;
    mutable std::string model_path_;
    mutable std::string last_error_;
    mutable std::size_t dimension_ = 384;
    mutable bool load_attempted_ = false;
};

} // namespace zipper::rag
