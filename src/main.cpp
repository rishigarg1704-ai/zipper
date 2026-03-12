#include <iostream>
#include <filesystem>

#include "core/runtime_manager.hpp"

int main() {

    const std::string model_name = "llama3_8b_q4.gguf";
    std::string model_path = "models/" + model_name;

    if (!std::filesystem::exists(model_path)) {
        model_path = "../models/" + model_name;
    }

    zipper::RuntimeManager runtime(
        model_path,
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

    runtime.set_system_prompt(
        "You are Zipper, a professional AI assistant. "
        "Respond clearly and concisely. "
        "Avoid exaggerated excitement or unnecessary phrases."
    );

    std::cout << "Zipper ready. Type 'exit' to quit, 'clear' to reset.\n";

    while (true) {

        std::cout << "\nYou: ";

        std::string input;
        std::getline(std::cin, input);

        if (input == "exit")
            break;

        if (input == "clear") {
            runtime.clear_conversation();
            std::cout << "Conversation cleared.\n";
            continue;
        }

        if (input.empty())
            continue;

        std::cout << "Assistant: ";

        runtime.generate(input, [](const std::string& token) {
            std::cout << token << std::flush;
        });

        std::cout << "\n";
    }

    runtime.unload_model();

    return 0;
}
