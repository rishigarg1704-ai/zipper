cmake_minimum_required(VERSION 3.20)

project(Zipper VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Add llama.cpp ----
add_subdirectory(external/llama.cpp)

set(SOURCE_FILES
    src/main.cpp

    src/core/runtime_manager.cpp
    src/core/conversation_manager.cpp
    src/core/prompt_builder.cpp
    src/core/context_manager.cpp


    src/backend/llama_backend.cpp
)

add_executable(Zipper ${SOURCE_FILES})

target_include_directories(Zipper PRIVATE
    src
    external/llama.cpp
    external/json
)

target_link_libraries(Zipper PRIVATE llama)