# Zipper AI — System Architecture

Founder: Rishi Garg
Project: Zipper AI
Language: C++
Runtime Backend: llama.cpp
Primary Platform: Apple Silicon (Metal / CPU)

---

# 1. Overview

Zipper is a **local AI runtime platform** designed to run entirely on a user's machine.

The goal of Zipper is to provide a **downloadable AI workspace** where users can:

* run local language models
* interact with AI tools
* build personal knowledge bases
* automate tasks with AI agents
* analyze documents
* run everything locally without cloud dependency

Zipper is distributed as a simple package:

```
zipper.zip
```

Users can download, extract, and run the application locally.

```
unzip zipper.zip
cd zipper
./Zipper
```

---

# 2. Core Architecture

The system follows a modular architecture built around a **runtime manager** that coordinates all components.

Main execution flow:

```
User
 ↓
RuntimeManager
 ↓
ToolParser
 ↓
ToolExecutor / RAG / LLM
 ↓
Assistant Response
```

---

# 3. Core Runtime System

The runtime system manages the entire AI execution pipeline.

Primary components:

```
RuntimeManager
ConversationManager
ContextManager
PromptBuilder
LlamaBackend
```

Responsibilities:

RuntimeManager

* orchestrates the entire AI workflow
* routes requests to tools, RAG, or LLM

ConversationManager

* stores conversation history
* manages assistant/user messages

ContextManager

* trims conversation to fit model context limits

PromptBuilder

* constructs final prompts sent to the model

LlamaBackend

* loads and executes the LLM using llama.cpp

---

# 4. Model Inference System

Zipper uses **GGUF models** running through llama.cpp.

Example model:

```
llama3_8b_q4.gguf
```

Inference pipeline:

```
Prompt
 ↓
Tokenization
 ↓
llama_decode
 ↓
Token sampling
 ↓
Streaming response
```

Supported features:

* streaming tokens
* configurable context size
* configurable threads
* adjustable sampling parameters

---

# 5. Tool System

Zipper includes a modular **tool execution framework**.

Tools allow the AI to perform external actions.

Architecture:

```
ToolParser
 ↓
ToolExecutor
 ↓
ToolRegistry
 ↓
Tool Implementation
```

Tool interface:

```
class Tool {
    name()
    description()
    execute(arguments)
}
```

Example tools:

```
calculator
web_search
stock
date
system_info
file_reader
```

Tool commands use explicit syntax:

```
/search AI news
/stock TSLA
/date
```

---

# 6. Retrieval-Augmented Generation (RAG)

RAG enables the AI to answer questions using **user-provided documents**.

Pipeline:

```
DocumentLoader
 ↓
TextChunker
 ↓
EmbeddingGenerator
 ↓
VectorStore
 ↓
Retriever
 ↓
ContextBuilder
 ↓
LLM
```

Components:

DocumentLoader

* loads user files (TXT / MD)

TextChunker

* splits documents into overlapping chunks

EmbeddingGenerator

* converts text into vector embeddings

VectorStore

* stores chunk vectors

Retriever

* performs similarity search

ContextBuilder

* builds prompt context from retrieved chunks

---

# 7. Embedding System (Phase 2)

Phase-2 replaces simple hash embeddings with **semantic embeddings**.

Selected model:

```
bge-small-en-v1.5
```

Embedding backend:

```
EmbeddingBackend
```

Responsibilities:

* load embedding model
* generate embeddings
* normalize vectors

Embedding interface:

```
std::vector<float> embed(const std::string& text);
```

Embedding dimension:

```
384
```

---

# 8. Vector Store

The vector store manages document embeddings.

Data stored for each chunk:

```
embedding vector
chunk text
document id
chunk id
source file
```

Similarity search uses:

```
cosine similarity
```

Retrieval parameters:

```
top_k = 5
similarity_threshold = 0.25
```

---

# 9. RAG Context Control

To prevent prompt overflow, the context builder limits injected document size.

Maximum context:

```
max_context_tokens = 1200
```

Token estimation:

```
tokens ≈ words × 1.3
```

Chunks are added until the limit is reached.

---

# 10. Persistent Knowledge Storage

RAG data is persisted locally.

Directory:

```
data/rag_index/
```

Files:

```
embeddings.bin
chunks.json
metadata.json
```

Startup behavior:

```
load index automatically
```

---

# 11. GUI Interface (Future Phase)

Zipper will provide a graphical interface for everyday users.

Main UI sections:

```
Chat
Knowledge Base
Files
Tools
Models
Settings
```

Example layout:

```
┌───────────────────────────┐
│ Zipper AI                 │
├───────────────┬───────────┤
│ Chat          │ Files     │
│ Knowledge     │ Models    │
│ Tools         │ Settings  │
└───────────────┴───────────┘
```

---

# 12. Model Management

Users will be able to install and manage models.

Model directory:

```
models/
```

Supported types:

```
chat models
embedding models
specialized models
```

Example models:

```
Llama
Mistral
DeepSeek
Phi
```

---

# 13. AI Agents (Future Phase)

Agents enable multi-step autonomous tasks.

Agent architecture:

```
Planner
 ↓
Tool Executor
 ↓
Memory
 ↓
LLM
```

Example tasks:

```
research topic
summarize documents
write reports
automate workflows
```

---

# 14. Plugin Ecosystem

Future versions will support external plugins.

Directory:

```
plugins/
```

Examples:

```
GitHub integration
email automation
calendar tools
custom tools
```

---

# 15. Distribution Model

Zipper is distributed as a local AI application.

Installation process:

```
Download zipper.zip
Extract archive
Run Zipper
```

No cloud services required.

All AI computation runs locally.

---

# 16. Engineering Principles

Development follows strict engineering rules.

Primary rules:

* never break working runtime
* maintain modular architecture
* avoid unnecessary dependencies
* prioritize local execution
* maintain clear system boundaries

---

# 17. Long-Term Vision

Zipper aims to become a **local AI workspace platform**.

Core capabilities:

```
local LLM runtime
personal knowledge engine
tool execution framework
AI agents
modular plugins
desktop workspace UI
```

The system ultimately becomes a **local AI operating layer** running entirely on the user's machine.

---

