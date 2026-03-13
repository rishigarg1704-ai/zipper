#pragma once

#include <string>

namespace zipper::rag {

struct DocumentLoadResult {
    bool success = false;
    std::string resolved_path;
    std::string text;
    std::string error;
};

class DocumentLoader {
public:
    DocumentLoadResult load_document(const std::string& path) const;
};

} // namespace zipper::rag
