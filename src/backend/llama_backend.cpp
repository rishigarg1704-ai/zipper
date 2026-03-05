#include "llama_backend.hpp"

#include <iostream>
#include <vector>

#include "../../external/llama.cpp/include/llama.h"

namespace zipper {

bool LlamaBackend::load_model(const std::string& model_path,
                               int context_size,
                               int n_threads) {

    llama_backend_init();

    llama_model_params model_params = llama_model_default_params();
    model_ = llama_model_load_from_file(model_path.c_str(), model_params);

    if (!model_) {
        std::cerr << "Failed to load model\n";
        return false;
    }

    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = context_size;
    ctx_params.n_threads = n_threads;
    ctx_params.n_threads_batch = n_threads;

    ctx_ = llama_init_from_model(model_, ctx_params);

    if (!ctx_) {
        std::cerr << "Failed to create context\n";
        return false;
    }

    return true;
}

void LlamaBackend::unload_model() {

    if (ctx_) {
        llama_free(ctx_);
        ctx_ = nullptr;
    }

    if (model_) {
        llama_model_free(model_);
        model_ = nullptr;
    }

    llama_backend_free();
}

bool LlamaBackend::generate(
    const std::string& prompt,
    int max_tokens,
    float temperature,
    float top_p,
    TokenCallback callback)
{
    if (!ctx_ || !model_)
        return false;

    const llama_vocab* vocab = llama_model_get_vocab(model_);

    std::vector<llama_token> tokens(prompt.size() + 32);

    int n_tokens = llama_tokenize(
        vocab,
        prompt.c_str(),
        prompt.length(),
        tokens.data(),
        tokens.size(),
        true,
        true
    );

    if (n_tokens < 0)
        return false;

    tokens.resize(n_tokens);

    // ---- decode prompt ----

    llama_batch batch = llama_batch_init(n_tokens, 0, 1);

    for (int i = 0; i < n_tokens; i++)
    {
        batch.token[i] = tokens[i];
        batch.pos[i] = i;
        batch.n_seq_id[i] = 1;
        batch.seq_id[i][0] = 0;
        batch.logits[i] = (i == n_tokens - 1);
    }

    batch.n_tokens = n_tokens;

    if (llama_decode(ctx_, batch) != 0)
    {
        llama_batch_free(batch);
        return false;
    }

    llama_batch_free(batch);

    // ---- sampler ----

    llama_sampler* sampler =
        llama_sampler_chain_init(llama_sampler_chain_default_params());

    llama_sampler_chain_add(sampler, llama_sampler_init_temp(temperature));
    llama_sampler_chain_add(sampler, llama_sampler_init_top_p(top_p, 1));
    llama_sampler_chain_add(sampler, llama_sampler_init_dist(0));  // REQUIRED!

    // ---- generation loop ----

    for (int i = 0; i < max_tokens; i++)
    {
        llama_token token = llama_sampler_sample(sampler, ctx_, -1);

        if (llama_vocab_is_eog(vocab, token))
            break;

        llama_sampler_accept(sampler, token);

        // Output token FIRST
        char piece[128];

        int len = llama_token_to_piece(
            vocab,
            token,
            piece,
            sizeof(piece),
            0,
            true
        );

        if (len > 0)
            callback(std::string(piece, len));

        // THEN decode next token
        llama_batch new_batch = llama_batch_init(1, 0, 1);

        new_batch.token[0] = token;
        new_batch.pos[0] = n_tokens + i;
        new_batch.n_seq_id[0] = 1;
        new_batch.seq_id[0][0] = 0;
        new_batch.logits[0] = true;
        new_batch.n_tokens = 1;

        if (llama_decode(ctx_, new_batch) != 0)
        {
            llama_batch_free(new_batch);
            break;
        }

        llama_batch_free(new_batch);
    }

    llama_sampler_free(sampler);

    return true;
}

}