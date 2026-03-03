# Zipper

Zipper is a high-performance local AI runtime built in modern C++ with Metal GPU acceleration.

It runs large language models fully offline using llama.cpp and provides a structured multi-turn chat engine with persistent session memory.

---

## 🚀 Features

- ⚡ Metal GPU acceleration (Apple Silicon optimized)
- 🧠 Llama 3.x GGUF model support
- 🔄 Multi-turn persistent chat sessions
- 🧹 Smart context trimming (turn-based)
- 💾 Auto-save session to JSON
- 🛡 Memory guard system
- 🧱 Modular architecture (core / backend / gui)

---

## 🏗 Architecture


- `core/` → engine logic
- `backend/` → model integration
- `gui/` → UI layer (future expansion)
- `external/` → third-party libraries
- `config/` → persistent session storage

---

## 📦 Requirements

- macOS (Apple Silicon recommended)
- CMake 3.20+
- Xcode Command Line Tools
- 16GB RAM recommended for 3B models

---

## 🛠 Build

```bash
git clone --recurse-submodules https://github.com/rishigarg1704-ai/zipper.git
cd zipper
mkdir build
cd build
cmake ..
make
./Zipper 

models/

models/llama3_3b_q4.gguf  


---
MIT License

Copyright (c) 2026 Rishi Garg

Permission is hereby granted...
