# Zipper AI — Project Engineering Rules

Founder: Rishi Garg
Project: Zipper AI Runtime
Language: C++
Runtime: llama.cpp
Hardware: Apple Silicon (Metal GPU)

This document defines strict engineering rules for anyone contributing to Zipper AI.

These rules exist to prevent architectural mistakes, broken builds, and unnecessary refactoring.

--------------------------------------------------

# 1. Primary Project Goal

Zipper is a modular local AI runtime platform.

The system must support:

- local LLM inference
- conversation memory
- context management
- tool execution
- internet tools
- knowledge retrieval (RAG)
- agents
- GUI interface

Architecture must remain modular and scalable.

--------------------------------------------------

# 2. Critical Rule: Never Break Working Systems

If something already works, it must not be broken.

Currently stable systems:

- llama backend
- runtime manager
- conversation manager
- context manager
- prompt builder
- tool registry
- tool executor
- calculator tool

Any change must preserve compatibility.

--------------------------------------------------

# 3. Code Output Rules

When writing code:

Always provide:

- FULL FILES
- correct includes
- namespaces
- build compatibility

Never provide partial patches unless explicitly requested.

--------------------------------------------------

# 4. Build System Rules

Build system is CMake.

Every new module must be added to:

CMakeLists.txt

Build must always succeed using:

mkdir build
cd build
cmake ..
make

The project must remain buildable on a clean machine.

--------------------------------------------------

# 5. Dependency Rules

Dependencies must remain local inside the project.

Allowed dependencies:

external/llama.cpp
external/json

Do NOT introduce new external dependencies unless necessary.

--------------------------------------------------

# 6. Path Safety

Never hardcode absolute paths.

Bad:

/Users/rishi/Desktop/zipper/models/model.gguf

Correct:

../models/model.gguf

The project must run after being zipped and moved to another machine.

--------------------------------------------------

# 7. Tool System Rules

All tools must follow the same architecture.

Every tool must:

1. inherit from Tool interface
2. register inside ToolRegistry
3. execute through ToolExecutor

Folder structure:

src/tools/

Example:

calculator/
web_search/
file_reader/

--------------------------------------------------

# 8. Runtime Architecture

The runtime pipeline must remain:

User
↓
RuntimeManager
↓
ConversationManager
↓
ContextManager
↓
PromptBuilder
↓
LlamaBackend
↓
Model Response

Tool execution pipeline:

User
↓
ToolParser
↓
ToolExecutor
↓
ToolRegistry
↓
Tool Implementation

--------------------------------------------------

# 9. Future Architecture (Planned)

Upcoming systems:

WebSearchTool
RAG knowledge system
persistent memory
agent planner
GUI interface
hardware auto-detection

These should integrate cleanly without breaking current modules.

--------------------------------------------------

# 10. Portable Packaging Requirement

The project must be distributable as:

zipper.zip

Another machine must be able to run:

unzip zipper.zip
cd zipper
mkdir build
cd build
cmake ..
make
./Zipper

No additional setup required.

--------------------------------------------------

# 11. Engineering Philosophy

Zipper should be engineered like a professional AI runtime system.

Priorities:

stability
modularity
readable C++
predictable architecture
long-term scalability

Avoid hacks and temporary solutions.

--------------------------------------------------

# 12. Assistant Role (for AI contributors)

AI assistants working on this project must act as:

Chief AI Systems Engineer

Responsibilities:

- maintain architecture
- avoid breaking stable components
- provide production-grade code
- explain design decisions
- prevent unnecessary refactoring

--------------------------------------------------
