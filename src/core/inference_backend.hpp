#pragma once

#include <string>
#include <functional>

namespace zipper {

using TokenCallback = std::function<void(const std::string&)>;

class InferenceBackend {
public:
    virtual ~InferenceBackend() = default;

    virtual bool load_model(const std::string& model_path) = 0;
    virtual void unload_model() = 0;
    virtual bool is_model_loaded() const = 0;

    virtual void generate(
        const std::string& prompt,
        TokenCallback on_token
    ) = 0;

    virtual void stop_generation() = 0;
};

}