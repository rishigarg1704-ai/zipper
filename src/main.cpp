#include <iostream>
#include <string>

#include "core/logger.hpp"
#include "core/crash_handler.hpp"
#include "core/runtime_manager.hpp"
#include "core/session_manager.hpp"

int main() {

    zipper::Logger::instance().init("zipper.log");
    zipper::CrashHandler::install();

    zipper::RuntimeManager runtime;
    zipper::SessionManager session;

    const std::string model_path = "../models/llama3_3b_q4.gguf";

    auto result = runtime.load_model(model_path);

    if (result != zipper::MemoryCheckResult::ALLOW) {
        std::cout << "Model load failed.\n";
        return 1;
    }

    std::cout << "Zipper ready.\n";
    std::cout << "Type '/reset' to clear memory.\n";
    std::cout << "Type 'exit' to quit.\n";

    while (true) {

        std::cout << "\nYou: ";

        std::string input;
        std::getline(std::cin >> std::ws, input);

        if (input == "exit") break;

        if (input == "/reset") {
            session.reset();
            std::cout << "Session reset.\n";
            continue;
        }

        if (input.empty()) continue;

        // 1️⃣ Add user message
        session.add_user_message(input);

        // 2️⃣ Build full conversation prompt
        std::string full_prompt = session.build_prompt();

        std::cout << "Assistant: ";

        std::string assistant_reply;

        // 3️⃣ Generate response
        runtime.generate(
            full_prompt,
            [&](const std::string& token) {
                std::cout << token << std::flush;
                assistant_reply += token;
            }
        );

        std::cout << "\n";

        // 4️⃣ Save assistant reply
        session.add_assistant_message(assistant_reply);
    }

    runtime.unload_model();
    zipper::Logger::instance().shutdown();

    return 0;
}