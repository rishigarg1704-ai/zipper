#include "embedding_backend.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <thread>
#include <vector>

#include "llama.h"

namespace zipper::rag {
namespace {

constexpr std::size_t kDefaultEmbeddingDimension = 384;
constexpr std::uint32_t kDefaultEmbeddingContext = 512;
constexpr char kEmbeddingModelStem[] = "bge-small-en-v1.5";
constexpr char kEmbeddingModelFile[] = "bge-small-en-v1.5.gguf";

std::string read_env_path()
{
    const char* env_value = std::getenv("ZIPPER_RAG_EMBEDDING_MODEL");
    if (!env_value || *env_value == '\0') {
        return {};
    }

    return std::string(env_value);
}

std::string resolve_embedding_model_path()
{
    const std::string env_path = read_env_path();
    if (!env_path.empty() && std::filesystem::exists(env_path)) {
        return env_path;
    }

    const std::vector<std::filesystem::path> direct_candidates = {
        std::filesystem::path("models") / kEmbeddingModelFile,
        std::filesystem::path("..") / "models" / kEmbeddingModelFile,
    };

    for (const auto& candidate : direct_candidates) {
        if (std::filesystem::exists(candidate)) {
            return candidate.string();
        }
    }

    const std::vector<std::filesystem::path> search_roots = {
        std::filesystem::path("models"),
        std::filesystem::path("..") / "models",
    };

    for (const auto& root : search_roots) {
        if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) {
            continue;
        }

        for (const auto& entry : std::filesystem::directory_iterator(root)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            const std::filesystem::path path = entry.path();
            const std::string filename = path.filename().string();
            if (path.extension() == ".gguf" &&
                filename.find(kEmbeddingModelStem) != std::string::npos) {
                return path.string();
            }
        }
    }

    return direct_candidates.front().string();
}

std::vector<llama_token> tokenize_text(const llama_vocab* vocab,
                                       const std::string& text)
{
    if (!vocab || text.empty()) {
        return {};
    }

    std::vector<llama_token> tokens(std::max<std::size_t>(text.size() + 8, 32));

    int32_t token_count = llama_tokenize(
        vocab,
        text.c_str(),
        static_cast<int32_t>(text.size()),
        tokens.data(),
        static_cast<int32_t>(tokens.size()),
        true,
        false);

    if (token_count < 0) {
        tokens.resize(static_cast<std::size_t>(-token_count));
        token_count = llama_tokenize(
            vocab,
            text.c_str(),
            static_cast<int32_t>(text.size()),
            tokens.data(),
            static_cast<int32_t>(tokens.size()),
            true,
            false);
    }

    if (token_count <= 0) {
        return {};
    }

    tokens.resize(static_cast<std::size_t>(token_count));

    const llama_token eos = llama_vocab_eos(vocab);
    if (eos != LLAMA_TOKEN_NULL && (tokens.empty() || tokens.back() != eos)) {
        tokens.push_back(eos);
    }

    return tokens;
}

int select_thread_count()
{
    const unsigned int detected = std::thread::hardware_concurrency();
    if (detected == 0U) {
        return 4;
    }

    return static_cast<int>(std::max(1U, std::min(detected, 8U)));
}

} // namespace

EmbeddingBackend::EmbeddingBackend()
    : model_path_(resolve_embedding_model_path()),
      dimension_(kDefaultEmbeddingDimension)
{
}

EmbeddingBackend::~EmbeddingBackend()
{
    unload();
}

std::vector<float> EmbeddingBackend::embed(const std::string& text) const
{
    if (text.empty()) {
        return zero_vector();
    }

    if (!ensure_loaded() || !model_ || !ctx_) {
        return zero_vector();
    }

    const llama_vocab* vocab = llama_model_get_vocab(model_);
    std::vector<llama_token> tokens = tokenize_text(vocab, text);
    if (tokens.empty()) {
        return zero_vector();
    }

    const std::size_t max_batch_tokens = static_cast<std::size_t>(llama_n_batch(ctx_));
    if (max_batch_tokens == 0U) {
        last_error_ = "Embedding context was created with an invalid batch size.";
        return zero_vector();
    }

    if (tokens.size() > max_batch_tokens) {
        tokens.resize(max_batch_tokens);

        const llama_token eos = llama_vocab_eos(vocab);
        if (eos != LLAMA_TOKEN_NULL && !tokens.empty()) {
            tokens.back() = eos;
        }
    }

    llama_memory_clear(llama_get_memory(ctx_), true);

    llama_batch batch = llama_batch_init(static_cast<int32_t>(tokens.size()), 0, 1);
    for (std::size_t i = 0; i < tokens.size(); ++i) {
        batch.token[i] = tokens[i];
        batch.pos[i] = static_cast<llama_pos>(i);
        batch.n_seq_id[i] = 1;
        batch.seq_id[i][0] = 0;
        batch.logits[i] = 1;
    }
    batch.n_tokens = static_cast<int32_t>(tokens.size());

    const bool requires_encode = llama_model_has_encoder(model_);
    const bool requires_decode = llama_model_has_decoder(model_);
    int32_t status = 0;

    if (requires_encode && requires_decode) {
        last_error_ = "Encoder-decoder embedding models are not supported.";
        llama_batch_free(batch);
        return zero_vector();
    }

    if (requires_encode) {
        status = llama_encode(ctx_, batch);
    } else {
        status = llama_decode(ctx_, batch);
    }

    llama_synchronize(ctx_);

    if (status != 0) {
        last_error_ = "Failed to run embedding model for input text.";
        llama_batch_free(batch);
        return zero_vector();
    }

    std::vector<float> embedding(dimension_, 0.0f);
    const enum llama_pooling_type pooling_type = llama_pooling_type(ctx_);

    if (pooling_type != LLAMA_POOLING_TYPE_NONE) {
        const float* seq_embedding = llama_get_embeddings_seq(ctx_, 0);
        if (seq_embedding) {
            std::copy(seq_embedding, seq_embedding + dimension_, embedding.begin());
        }
    } else {
        std::size_t used_embeddings = 0;
        for (std::size_t i = 0; i < tokens.size(); ++i) {
            const float* token_embedding = llama_get_embeddings_ith(ctx_, static_cast<int32_t>(i));
            if (!token_embedding) {
                continue;
            }

            for (std::size_t dim = 0; dim < dimension_; ++dim) {
                embedding[dim] += token_embedding[dim];
            }
            ++used_embeddings;
        }

        if (used_embeddings > 0U) {
            const float inv_count = 1.0f / static_cast<float>(used_embeddings);
            for (float& value : embedding) {
                value *= inv_count;
            }
        }
    }

    llama_batch_free(batch);
    normalize(embedding);
    return embedding;
}

bool EmbeddingBackend::ensure_loaded() const
{
    if (model_ && ctx_) {
        return true;
    }

    if (load_attempted_) {
        return false;
    }

    load_attempted_ = true;
    return load_model();
}

bool EmbeddingBackend::is_ready() const
{
    return model_ != nullptr && ctx_ != nullptr;
}

std::size_t EmbeddingBackend::dimension() const
{
    return dimension_;
}

const std::string& EmbeddingBackend::model_path() const
{
    return model_path_;
}

const std::string& EmbeddingBackend::last_error() const
{
    return last_error_;
}

void EmbeddingBackend::unload()
{
    if (ctx_) {
        llama_free(ctx_);
        ctx_ = nullptr;
    }

    if (model_) {
        llama_model_free(model_);
        model_ = nullptr;
    }

    last_error_.clear();
    load_attempted_ = false;
    dimension_ = kDefaultEmbeddingDimension;
}

bool EmbeddingBackend::load_model() const
{
    model_path_ = resolve_embedding_model_path();

    if (model_path_.empty() || !std::filesystem::exists(model_path_)) {
        last_error_ = "Embedding model not found: " + model_path_;
        return false;
    }

    llama_backend_init();

    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = 0;

    model_ = llama_model_load_from_file(model_path_.c_str(), model_params);
    if (!model_) {
        last_error_ = "Failed to load embedding model: " + model_path_;
        return false;
    }

    llama_context_params ctx_params = llama_context_default_params();
    const int32_t trained_context = llama_model_n_ctx_train(model_);
    const uint32_t context_tokens = trained_context > 0
                                        ? static_cast<uint32_t>(std::min<int32_t>(
                                              trained_context,
                                              static_cast<int32_t>(kDefaultEmbeddingContext)))
                                        : kDefaultEmbeddingContext;

    ctx_params.n_ctx = std::max<std::uint32_t>(context_tokens, 32U);
    ctx_params.n_batch = ctx_params.n_ctx;
    ctx_params.n_ubatch = ctx_params.n_ctx;
    ctx_params.n_seq_max = 1;
    ctx_params.n_threads = select_thread_count();
    ctx_params.n_threads_batch = ctx_params.n_threads;
    ctx_params.embeddings = true;

    if (llama_model_has_encoder(model_) && !llama_model_has_decoder(model_)) {
        ctx_params.attention_type = LLAMA_ATTENTION_TYPE_NON_CAUSAL;
    }

    ctx_ = llama_init_from_model(model_, ctx_params);
    if (!ctx_) {
        last_error_ = "Failed to create embedding context for model: " + model_path_;
        llama_model_free(model_);
        model_ = nullptr;
        return false;
    }

    const int32_t embedding_dimension = llama_model_n_embd_out(model_);
    if (embedding_dimension > 0) {
        dimension_ = static_cast<std::size_t>(embedding_dimension);
    }

    last_error_.clear();
    return true;
}

std::vector<float> EmbeddingBackend::zero_vector() const
{
    return std::vector<float>(dimension_, 0.0f);
}

void EmbeddingBackend::normalize(std::vector<float>& values)
{
    float norm = 0.0f;
    for (float value : values) {
        norm += value * value;
    }

    norm = std::sqrt(norm);
    if (norm <= 0.0f) {
        return;
    }

    for (float& value : values) {
        value /= norm;
    }
}

} // namespace zipper::rag
