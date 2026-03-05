#pragma once

#include <string>
#include <functional>

struct llama_model;
struct llama_context;

namespace zipper {

class LlamaBackend {
public:

    using TokenCallback = std::function<void(const std::string&)>;

    LlamaBackend() = default;
    ~LlamaBackend() = default;

    bool load_model(const std::string& model_path,
                    int context_size,
                    int n_threads);

    void unload_model();

    bool generate(const std::string& prompt,
                  int max_tokens,
                  float temperature,
                  float top_p,
                  TokenCallback callback);

private:
    llama_model* model_ = nullptr;
    llama_context* ctx_ = nullptr;
};

}