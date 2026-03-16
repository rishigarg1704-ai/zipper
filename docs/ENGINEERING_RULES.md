# Zipper AI — Engineering Rules

This document defines strict engineering rules for developing the Zipper runtime.

These rules exist to protect the architecture, maintain stability, and ensure the project can grow into a reliable AI platform.


--------------------------------------------------

1. Stability First

Working systems must never be broken unnecessarily.

If a component already works:

- avoid rewriting it
- extend it instead
- make incremental improvements

Large refactors must be carefully justified.


--------------------------------------------------

2. Modular Architecture

Every subsystem must be modular.

Modules should be:

- independent
- replaceable
- testable

Core runtime modules include:

- Model Engine
- Tool Engine
- RAG Engine
- Agent Engine
- Memory Engine

Modules must communicate through clear interfaces.


--------------------------------------------------

3. Runtime Independence

The runtime must remain independent from user interfaces.

The following must never contain runtime logic:

- CLI interface
- GUI workspace
- API endpoints

Correct architecture:

User Interface
↓
Runtime API
↓
Zipper Runtime


--------------------------------------------------

4. Tool Interface Consistency

All tools must implement the Tool interface.

Example structure:

Tool  
↓  
ToolRegistry  
↓  
ToolExecutor  
↓  
Tool Implementation

New tools must register themselves with ToolRegistry.


--------------------------------------------------

5. No Hardcoded Paths

All file paths must be portable.

Bad example:

/Users/rishi/Desktop/zipper/models/model.gguf

Correct example:

../models/model.gguf

This ensures the project works when moved to another machine.


--------------------------------------------------

6. Minimal External Dependencies

Zipper must remain lightweight and portable.

Allowed dependencies should be limited.

Current dependencies:

- llama.cpp
- nlohmann/json

New dependencies must be carefully reviewed.


--------------------------------------------------

7. Build System Stability

The project must always compile using the standard build commands.

Required build steps:

cmake -S . -B build  
cmake --build build  

The build must succeed on a clean machine.


--------------------------------------------------

8. Source Code Organization

Source code must follow a clear structure.

Example layout:

src/

backend/  
core/  
tools/  
rag/  
agents/  

Documentation must live in:

docs/


--------------------------------------------------

9. Logging and Error Handling

All runtime systems must handle errors safely.

Requirements:

- clear error messages
- graceful failure
- no silent crashes

Failures should never corrupt user data.


--------------------------------------------------

10. Security and Safety

Because Zipper can access system resources, safety controls are required.

Examples:

- confirm destructive actions
- restrict unsafe tool usage
- log sensitive operations


--------------------------------------------------

11. Incremental Development

Development must follow the defined roadmap.

Phases:

1. Runtime Engine
2. Basic RAG
3. Intelligent RAG
4. Agent System
5. GUI Workspace
6. Memory System
7. Plugin Ecosystem

Skipping phases risks breaking the architecture.


--------------------------------------------------

12. Documentation Requirement

Major features must be documented.

Required documentation includes:

- architecture changes
- new modules
- tool interfaces
- agent systems

Documentation must be updated alongside code.


--------------------------------------------------

13. Performance Awareness

Zipper runs on local machines, so efficiency matters.

Engineers should prioritize:

- low memory usage
- fast inference
- efficient retrieval
- minimal latency

Performance regressions should be avoided.


--------------------------------------------------

14. Code Clarity

Code should prioritize readability.

Guidelines:

- meaningful function names
- consistent formatting
- clear comments for complex logic

Future developers and AI assistants must be able to understand the code easily.


--------------------------------------------------

15. Long-Term Goal

The engineering goal is to build a stable AI runtime platform capable of supporting:

- local AI assistants
- research tools
- coding agents
- knowledge systems
- automation tools

All running locally on user machines.
