#pragma once

#include "core/inference_backend.hpp"
#include "core/memory_manager.hpp"
#include "core/session_manager.hpp"

#include <memory>
#include <atomic>
#include <mutex>
#include <thread>

namespace zipper {

class RuntimeManager {
public:
    RuntimeManager();
    ~RuntimeManager();

    MemoryCheckResult load_model(const std::string& model_path);
    void enable_memory_override();

    void unload_model();
    bool is_model_loaded() const;

    void send_user_message(const std::string& message);

    void generate(TokenCallback on_token);
    void stop_generation();

private:
    std::unique_ptr<InferenceBackend> backend_;
    MemoryManager memory_manager_;
    SessionManager session_manager_;

    std::atomic<bool> generating_;
    std::atomic<bool> stop_requested_;

    std::mutex mutex_;
    std::thread worker_thread_;
};

}