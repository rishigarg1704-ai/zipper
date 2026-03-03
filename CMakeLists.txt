cmake_minimum_required(VERSION 3.20)

project(Zipper VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Add llama.cpp ----
add_subdirectory(external/llama.cpp)

set(SOURCE_FILES
    src/main.cpp

    src/core/app_controller.cpp
    src/core/runtime_manager.cpp
    src/core/memory_manager.cpp
    src/core/session_manager.cpp
    src/core/logger.cpp
    src/core/crash_handler.cpp

    src/backend/llama_backend.cpp

    src/gui/main_window.cpp
    src/gui/input_panel.cpp
    src/gui/output_panel.cpp
    src/gui/ui_state.cpp
)

add_executable(Zipper ${SOURCE_FILES})

target_include_directories(Zipper PRIVATE
    src
    external/llama.cpp
)

target_link_libraries(Zipper PRIVATE llama)