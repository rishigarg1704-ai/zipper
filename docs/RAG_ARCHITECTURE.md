# Zipper AI — RAG Architecture

This document describes the Retrieval-Augmented Generation (RAG) system used by Zipper.

The RAG engine allows Zipper to answer questions using documents stored locally on the user’s machine.


--------------------------------------------------

1. Purpose of RAG

Large language models cannot reliably remember large document collections.

RAG solves this by retrieving relevant information from documents and injecting it into the prompt before the model generates an answer.

Benefits:

- accurate answers from documents
- reduced hallucination
- scalable knowledge systems
- support for large document libraries


--------------------------------------------------

2. Basic RAG Pipeline

The RAG pipeline consists of several stages.

Document
↓
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
LLM Response


--------------------------------------------------

3. Document Ingestion

Documents are loaded using the DocumentLoader component.

Supported formats initially include:

- txt
- md

Future support may include:

- pdf
- docx
- html


--------------------------------------------------

4. Chunking

Documents are divided into smaller chunks to allow efficient retrieval.

Current chunk settings:

- chunk size ≈ 500 tokens
- overlap ≈ 50 tokens

Chunking ensures the model receives manageable pieces of context.


--------------------------------------------------

5. Embedding Generation

Each chunk is converted into a vector representation.

Embedding model:

bge-small-en-v1.5

Embedding dimension:

384

Embeddings allow semantic search using vector similarity.


--------------------------------------------------

6. Vector Storage

Chunk embeddings are stored inside the VectorStore.

Stored data includes:

- chunk text
- embedding vector
- document ID
- chunk ID
- source file name

Current storage approach:

- in-memory vector store
- optional persistence on disk


--------------------------------------------------

7. Retrieval

When a user asks a question:

1. The query is converted into an embedding.
2. Vector similarity search finds relevant chunks.
3. The top results are selected.

Retrieval parameters:

- top_k = 5
- similarity threshold ≈ 0.25


--------------------------------------------------

8. Context Building

Retrieved chunks are combined into a context block.

Example prompt format:

Use the following context to answer the question.

Context:

[doc: manual.txt | chunk: 3]
<chunk text>

[doc: manual.txt | chunk: 7]
<chunk text>

Question:
<user query>

The context is then injected into the model prompt.


--------------------------------------------------

9. Intelligent RAG (Future Improvements)

Phase 3 introduces several improvements.

Planned enhancements:

- query rewriting
- hybrid retrieval (vector + keyword)
- candidate pool retrieval
- reranking
- context compression


--------------------------------------------------

10. Future Retrieval Pipeline

User Query
↓
Query Rewrite
↓
Hybrid Retrieval
↓
Candidate Pool (≈30 chunks)
↓
Reranker
↓
Context Compression
↓
Prompt Builder
↓
LLM Response


--------------------------------------------------

11. Scaling Strategy

As the knowledge base grows, the RAG system must scale.

Future improvements may include:

- disk-based vector indexes
- approximate nearest neighbor search
- document metadata filtering
- caching of frequent queries


--------------------------------------------------

12. Design Principles

The Zipper RAG system follows several principles:

- local-first knowledge
- modular design
- efficient retrieval
- minimal hallucination
- scalable architecture


--------------------------------------------------

13. Long-Term Vision

The RAG engine will eventually support:

- multi-document reasoning
- research assistants
- document analysis
- knowledge graphs
- project-level knowledge bases

This will make Zipper a powerful local research and knowledge system.
