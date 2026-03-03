#include "core/runtime_manager.hpp"
#include "backend/llama_backend.hpp"
#include "core/logger.hpp"

namespace zipper {

RuntimeManager::RuntimeManager()
    : backend_(std::make_unique<LlamaBackend>()),
      generating_(false),
      stop_requested_(false)
{
    Logger::instance().log(LogLevel::INFO, "RuntimeManager created");
}

RuntimeManager::~RuntimeManager() {
    stop_generation();

    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }

    unload_model();
    Logger::instance().log(LogLevel::INFO, "RuntimeManager destroyed");
}

MemoryCheckResult RuntimeManager::load_model(const std::string& model_path) {
    std::lock_guard<std::mutex> lock(mutex_);

    auto result = memory_manager_.can_load_model(model_path);

    if (result != MemoryCheckResult::ALLOW) {
        Logger::instance().log(LogLevel::WARN, "Memory check did not allow model load");
        return result;
    }

    if (backend_->is_model_loaded()) {
        return MemoryCheckResult::ALLOW;
    }

    bool ok = backend_->load_model(model_path);

    if (ok) {
        Logger::instance().log(LogLevel::INFO, "Model load successful");
        return MemoryCheckResult::ALLOW;
    }

    Logger::instance().log(LogLevel::ERROR, "Backend failed to load model");
    return MemoryCheckResult::REFUSE;
}

void RuntimeManager::enable_memory_override() {
    memory_manager_.enable_override();
}

void RuntimeManager::unload_model() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (backend_->is_model_loaded()) {
        backend_->unload_model();
        Logger::instance().log(LogLevel::INFO, "Model unloaded");
    }
}

bool RuntimeManager::is_model_loaded() const {
    return backend_->is_model_loaded();
}

void RuntimeManager::send_user_message(const std::string& message) {
    session_manager_.add_user_message(message);
}

void RuntimeManager::generate(TokenCallback on_token) {
    if (!backend_->is_model_loaded()) {
        Logger::instance().log(LogLevel::ERROR,
            "Generate called without model loaded");
        return;
    }

    if (generating_) {
        Logger::instance().log(LogLevel::WARN,
            "Generation already in progress");
        return;
    }

    generating_ = true;
    stop_requested_ = false;

    std::string full_prompt = session_manager_.build_prompt();

    worker_thread_ = std::thread([this, full_prompt, on_token]() {
        std::string assistant_output;

        backend_->generate(full_prompt, [&](const std::string& token) {
            if (stop_requested_) return;

            assistant_output += token;
            on_token(token);
        });

        session_manager_.add_assistant_message(assistant_output);

        generating_ = false;
    });
}

void RuntimeManager::stop_generation() {
    if (generating_) {
        stop_requested_ = true;
        backend_->stop_generation();

        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }

        generating_ = false;
        Logger::instance().log(LogLevel::INFO,
            "RuntimeManager stopped generation");
    }
}

}