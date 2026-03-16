# Zipper AI — Project Roadmap

This document describes the long-term development plan for the Zipper AI runtime platform.

The roadmap is divided into phases. Each phase builds on the previous one and expands the system's capabilities.


--------------------------------------------------

Phase 1 — Core Runtime Engine

Status: Completed

Goal:

Create a stable local AI runtime capable of running large language models.

Main components:

- LlamaBackend
- RuntimeManager
- ConversationManager
- ContextManager
- PromptBuilder

Capabilities:

- GGUF model loading
- token streaming
- conversation history
- system prompts
- context trimming
- Metal GPU acceleration

Outcome:

Zipper can run a local AI assistant.


--------------------------------------------------

Phase 2 — Basic RAG (Knowledge Engine)

Status: Completed

Goal:

Allow the assistant to read and answer questions from documents.

Components implemented:

- DocumentLoader
- TextChunker
- EmbeddingBackend
- VectorStore
- Retriever
- ContextBuilder

Capabilities:

- load document
- rag status
- rag clear
- document-based answers
- source citations

Outcome:

Zipper can answer questions using local documents.


--------------------------------------------------

Phase 3 — Intelligent RAG

Status: Planned

Goal:

Improve retrieval accuracy and reduce hallucination.

New components to implement:

- query rewriting
- hybrid retrieval (vector + keyword search)
- candidate pool retrieval
- reranking system
- context compression
- similarity threshold tuning

Future retrieval pipeline:

User Query
↓
Query Rewrite
↓
Hybrid Retrieval
↓
Candidate Pool
↓
Reranking
↓
Context Compression
↓
Prompt Builder
↓
LLM Response

Outcome:

High-quality document reasoning.


--------------------------------------------------

Phase 4 — Agent System

Goal:

Enable Zipper to perform complex multi-step tasks.

Agent architecture:

Goal
↓
Planner
↓
Tool Execution
↓
Observation
↓
Iteration
↓
Final Result

Example agents:

- research agent
- coding assistant
- document analysis agent
- automation agent

Outcome:

Zipper becomes capable of autonomous problem solving.


--------------------------------------------------

Phase 5 — GUI Workspace

Goal:

Create a user-friendly desktop interface.

Possible frameworks:

- Tauri
- Electron
- Qt

Workspace features:

- chat interface
- document manager
- tool panels
- agent control
- model manager

Important rule:

The GUI must never contain runtime logic.

Outcome:

Zipper becomes usable as a desktop AI application.


--------------------------------------------------

Phase 6 — Memory System

Goal:

Allow Zipper to remember user preferences and project context.

Types of memory:

- conversation memory
- user preference memory
- project memory
- knowledge memory

Example behavior:

User opens Zipper
↓
Zipper recalls previous project
↓
Suggests continuing work

Outcome:

Zipper behaves like a personal AI assistant.


--------------------------------------------------

Phase 7 — Plugin Ecosystem

Goal:

Allow developers to extend Zipper.

Plugin types:

- tools
- agents
- knowledge connectors
- integrations

Example plugin structure:

plugins/

github_tool
research_agent
finance_connector
database_agent

Outcome:

Zipper becomes an extensible AI platform.


--------------------------------------------------

Long-Term Vision

The final Zipper platform will include:

- local AI runtime
- advanced knowledge retrieval
- intelligent agents
- personal memory
- extensible plugins
- graphical workspace

Applications powered by Zipper may include:

- research assistants
- coding assistants
- document analysis tools
- automation systems
- AI productivity workspaces

All running locally on user hardware.
