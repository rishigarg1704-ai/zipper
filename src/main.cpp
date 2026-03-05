#include <iostream>

#include "core/runtime_manager.hpp"

int main() {

    zipper::RuntimeManager runtime(
        "../models/llama3_8b_q4.gguf",
        4096,
        8,
        256,
        0.7f,
        0.9f
    );

    if (!runtime.load_model()) {
        std::cerr << "Model load failed.\n";
        return 1;
    }

    runtime.set_system_prompt("You are a helpful AI assistant named Zipper.");

    std::cout << "Zipper ready. Type 'exit' to quit, 'clear' to reset.\n";

    while (true) {

        std::cout << "\nYou: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit") break;
        if (input == "clear") {
            runtime.clear_conversation();
            std::cout << "Conversation cleared.\n";
            continue;
        }
        if (input.empty()) continue;

        std::cout << "Assistant: ";

        runtime.generate(input, [](const std::string& token) {
            std::cout << token << std::flush;
        });

        std::cout << "\n";
    }

    runtime.unload_model();

    return 0;
}