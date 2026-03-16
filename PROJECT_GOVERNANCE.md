# Zipper AI — Project Governance

Founder: Rishi Garg  
Project: Zipper AI Runtime Platform  
Language: C++  
Backend: llama.cpp  

---

# 1. Vision

Zipper is designed to be a **local AI runtime platform** that runs advanced AI systems directly on user hardware.

The goal is to build a system capable of powering:

- personal AI assistants
- research tools
- coding assistants
- knowledge systems
- automation agents

All running **locally and privately**.

---

# 2. Core Philosophy

Zipper follows several fundamental principles.

### Local First

The system prioritizes:

- local models
- local knowledge
- local execution

No external APIs should be required for core functionality.

---

### Modular Architecture

Each system component must be independent and replaceable.

Major engines:

- Model Engine
- RAG Knowledge Engine
- Tool Engine
- Agent Engine
- Memory Engine

---

### Stability First

Working systems must never be broken unnecessarily.

Changes must be incremental and maintain backward compatibility.

---

### Developer Platform

Zipper is not just an application.

It is intended to become a **platform for developers** to build AI-powered tools.

Developers should be able to extend Zipper by adding:

- tools
- agents
- knowledge connectors
- plugins

---

# 3. Core Architecture

The system architecture is layered.

User Interfaces:

- CLI
- GUI
- API

These interact with the runtime.

Runtime structure:

User  
↓  
Interface Layer  
↓  
Runtime Core  

Runtime Engines:

- Model Engine
- Tool Engine
- RAG Engine
- Agent Engine
- Memory Engine

---

# 4. Runtime Engines

## Model Engine

Responsible for:

- loading models
- GPU acceleration
- token streaming
- context management

Backend:

llama.cpp

---

## RAG Knowledge Engine

Responsible for:

- document ingestion
- embedding generation
- vector storage
- knowledge retrieval

---

## Tool Engine

Responsible for executing system tools.

Examples:

- web search
- file reader
- calculator
- system information

---

## Agent Engine

Responsible for multi-step reasoning and automation.

Agents can:

- plan tasks
- execute tools
- iterate until task completion

---

## Memory Engine

Responsible for persistent knowledge about:

- users
- projects
- conversations
- workflows

---

# 5. Safety Principles

Because Zipper can access local resources, safety rules are required.

Examples:

- dangerous actions require confirmation
- system access is limited
- actions are logged

---

# 6. Development Philosophy

Development should follow a phased approach.

Phases:

1. Runtime Engine
2. Basic RAG
3. Intelligent RAG
4. Agent System
5. GUI Workspace
6. Memory System
7. Plugin Ecosystem

Each phase must be completed before moving to the next.

---

# 7. Project Goal

The long-term goal is to create a system that acts as a **personal AI operating companion**.

Zipper should feel like:

- a research assistant
- a coding partner
- a document analyst
- a helpful personal AI

All running locally.
