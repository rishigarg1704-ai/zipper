cmake_minimum_required(VERSION 3.20)

project(Zipper)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GGML_METAL OFF CACHE BOOL "Disable Metal backend" FORCE)

add_subdirectory(external/llama.cpp)

add_executable(Zipper
    src/main.cpp
    src/backend/llama_backend.cpp
    src/core/runtime_manager.cpp
    src/core/conversation_manager.cpp
    src/core/context_manager.cpp
    src/core/prompt_builder.cpp
    src/core/system_prompt.cpp
    src/tools/tool_registry.cpp
    src/tools/tool_executor.cpp
    src/tools/tool_parser.cpp
    src/tools/calculator/calculator_tool.cpp
    src/tools/date/date_tool.cpp
    src/tools/system_info/system_info_tool.cpp
    src/tools/file_reader/file_reader_tool.cpp
    src/tools/stock/stock_tool.cpp
    src/tools/web_search/web_search_tool.cpp
)

target_include_directories(Zipper PRIVATE
    src
    src/backend
    src/core
    src/tools
    external/llama.cpp/include
    external/json
)

target_link_libraries(Zipper PRIVATE
    llama
)
