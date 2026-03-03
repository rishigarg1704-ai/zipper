#pragma once

#include <string>
#include <functional>

struct llama_model;
struct llama_context;

namespace zipper {

class LlamaBackend {
public:
    LlamaBackend();
    ~LlamaBackend();

    bool load(const std::string& model_path);
    void unload();

    void generate(
        const std::string& prompt,
        std::function<void(const std::string&)> callback,
        int max_tokens = 512,
        float temperature = 0.7f,
        float top_p = 0.9f
    );

private:
    llama_model* model_;
    llama_context* ctx_;
};

}