#include "llama_backend.hpp"
#include "../core/logger.hpp"

#include <vector>
#include <iostream>

#include "llama.h"

namespace zipper {

LlamaBackend::LlamaBackend()
    : model_(nullptr), ctx_(nullptr) {}

LlamaBackend::~LlamaBackend() {
    unload();
}

bool LlamaBackend::load(const std::string& model_path) {

    llama_backend_init();

    llama_model_params model_params = llama_model_default_params();
    model_ = llama_model_load_from_file(model_path.c_str(), model_params);

    if (!model_) {
        Logger::instance().log(LogLevel::ERROR, "Model load failed");
        return false;
    }

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = 4096;
    ctx_params.n_batch = 512;

    ctx_ = llama_init_from_model(model_, ctx_params);

    if (!ctx_) {
        Logger::instance().log(LogLevel::ERROR, "Context creation failed");
        return false;
    }

    Logger::instance().log(LogLevel::INFO, "Model loaded successfully");
    return true;
}

void LlamaBackend::unload() {

    if (ctx_) {
        llama_free(ctx_);
        ctx_ = nullptr;
    }

    if (model_) {
        llama_model_free(model_);
        model_ = nullptr;
    }
}

void LlamaBackend::generate(
    const std::string& prompt,
    std::function<void(const std::string&)> callback,
    int max_tokens,
    float temperature,
    float top_p
) {

    // 🔥 RESET CONTEXT (Stateless Mode)
    llama_kv_cache_clear(ctx_);

    const llama_vocab* vocab = llama_model_get_vocab(model_);

    // ---- TOKENIZE INPUT ----
    std::vector<llama_token> tokens(prompt.size() + 8);

    int n_tokens = llama_tokenize(
        vocab,
        prompt.c_str(),
        prompt.size(),
        tokens.data(),
        tokens.size(),
        true,
        true
    );

    tokens.resize(n_tokens);

    // ---- INITIAL DECODE ----
    llama_batch batch = llama_batch_init(n_tokens, 0, 1);

    for (int i = 0; i < n_tokens; ++i) {
        batch.token[i] = tokens[i];
        batch.pos[i] = i;
        batch.n_seq_id[i] = 1;
        batch.seq_id[i][0] = 0;
        batch.logits[i] = (i == n_tokens - 1);
    }

    batch.n_tokens = n_tokens;

    if (llama_decode(ctx_, batch) != 0) {
        Logger::instance().log(LogLevel::ERROR, "Initial decode failed");
        return;
    }

    // ---- SAMPLER ----
    llama_sampler* sampler = llama_sampler_chain_init();

    llama_sampler_chain_add(sampler, llama_sampler_init_top_p(top_p, 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_temp(temperature));

    // ---- GENERATION LOOP ----
    for (int i = 0; i < max_tokens; ++i) {

        llama_token new_token = llama_sampler_sample(sampler, ctx_, -1);

        if (new_token == llama_vocab_eos(vocab)) {
            break;
        }

        char piece[256];
        int len = llama_token_to_piece(
            vocab,
            new_token,
            piece,
            sizeof(piece),
            0,
            true
        );

        if (len > 0) {
            callback(std::string(piece, len));
        }

        llama_batch next_batch = llama_batch_init(1, 0, 1);

        next_batch.token[0] = new_token;
        next_batch.pos[0] = n_tokens + i;
        next_batch.n_seq_id[0] = 1;
        next_batch.seq_id[0][0] = 0;
        next_batch.logits[0] = 1;
        next_batch.n_tokens = 1;

        if (llama_decode(ctx_, next_batch) != 0) {
            break;
        }
    }

    llama_sampler_free(sampler);
}

} // namespace zipper