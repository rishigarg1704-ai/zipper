# Zipper AI — Development Context

This document provides a complete technical overview of the Zipper AI project so that future AI assistants and developers can understand the system without breaking existing architecture.

Founder: Rishi Garg  
Project: Zipper AI Runtime Platform  
Primary Language: C++  
LLM Backend: llama.cpp  
Primary Development Machine: MacBook Air M2  


--------------------------------------------------

PROJECT GOAL

Zipper is a modular local AI runtime platform designed to run advanced AI systems directly on user hardware.

The platform aims to support:

- local AI assistants
- knowledge retrieval systems
- intelligent agents
- automation tools
- research assistants

All functionality must run locally without requiring external APIs.


--------------------------------------------------

CURRENT PROJECT STATUS

Completed phases:

Phase 1 — Runtime Engine  
Phase 2 — Basic RAG Knowledge System

The system currently supports:

- GGUF model loading
- token streaming
- conversation memory
- context trimming
- system prompts
- tool execution
- document-based question answering


--------------------------------------------------

CORE SYSTEM ARCHITECTURE

Zipper is composed of several independent engines.

Zipper Runtime

- Model Engine
- Tool Engine
- RAG Knowledge Engine
- Agent Engine (planned)
- Memory Engine (planned)

Interfaces interact with the runtime:

User  
↓  
CLI / GUI / API  
↓  
Runtime Core  


--------------------------------------------------

MODEL ENGINE

The model engine handles AI model execution.

Backend:

llama.cpp

Capabilities:

- load GGUF models
- manage inference
- stream tokens
- manage context window
- GPU acceleration when available

Example model used:

llama3_8b_q4.gguf


--------------------------------------------------

TOOL ENGINE

The tool system allows the AI to perform actions outside the model.

Architecture:

Tool  
↓  
ToolRegistry  
↓  
ToolExecutor  
↓  
Tool Implementation

Example tools currently implemented:

- calculator
- web search
- system information
- file reader
- stock lookup


--------------------------------------------------

RAG KNOWLEDGE ENGINE

The Retrieval-Augmented Generation system allows Zipper to answer questions using documents.

Current pipeline:

Document
↓
DocumentLoader
↓
TextChunker
↓
Embedding Generator
↓
VectorStore
↓
Retriever
↓
ContextBuilder
↓
LLM Response

Embedding model currently used:

bge-small-en-v1.5

Embedding dimension:

384


--------------------------------------------------

CURRENT RAG CAPABILITIES

Users can interact with the knowledge system using commands.

Examples:

load document README.md

rag status

rag clear

The system:

- chunks documents
- generates embeddings
- stores vectors
- retrieves relevant chunks
- injects context into prompts


--------------------------------------------------

FUTURE RAG IMPROVEMENTS

Phase 3 will improve retrieval quality.

Planned features:

- query rewriting
- hybrid retrieval (vector + keyword)
- candidate pool retrieval
- reranking
- context compression


--------------------------------------------------

RUNTIME FILE STRUCTURE

The project structure follows this layout.

src/

backend/
core/
tools/
rag/
agent/ (planned)

external/

llama.cpp

docs/

system documentation

models/

GGUF models

data/

runtime data


--------------------------------------------------

BUILD INSTRUCTIONS

Build the project:

cmake -S . -B build
cmake --build build

Run the runtime:

./build/Zipper


--------------------------------------------------

ENGINEERING PRINCIPLES

The project follows strict engineering principles.

Key rules:

- never break working runtime systems
- maintain modular architecture
- avoid unnecessary dependencies
- maintain portability
- avoid hardcoded paths
- maintain clear documentation


--------------------------------------------------

LONG TERM ROADMAP

Future phases include:

Phase 3 — Intelligent RAG  
Phase 4 — Agent System  
Phase 5 — GUI Workspace  
Phase 6 — Memory System  
Phase 7 — Plugin Ecosystem  


--------------------------------------------------

DEVELOPMENT PHILOSOPHY

Zipper is designed to become a platform rather than a single application.

Developers should be able to build:

- AI assistants
- research tools
- coding assistants
- automation agents
- knowledge platforms

All powered by the Zipper runtime.


--------------------------------------------------

IMPORTANT RULE FOR AI ASSISTANTS

When working on this project:

- do not break working runtime systems
- do not change architecture without reason
- prefer modular additions over refactoring
- maintain compatibility with the existing codebase
- ensure the project always builds successfully
