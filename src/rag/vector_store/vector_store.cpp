#include "vector_store.hpp"

#include "../../../external/json/json.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <set>
#include <tuple>

namespace zipper::rag {
namespace {

using json = nlohmann::json;

std::string extract_source_file(const std::string& document_id)
{
    const std::filesystem::path path(document_id);
    const std::string filename = path.filename().string();
    return filename.empty() ? document_id : filename;
}

std::filesystem::path index_directory()
{
    return std::filesystem::path("data") / "rag_index";
}

std::filesystem::path embeddings_file_path()
{
    return index_directory() / "embeddings.bin";
}

std::filesystem::path chunks_file_path()
{
    return index_directory() / "chunks.json";
}

std::filesystem::path metadata_file_path()
{
    return index_directory() / "metadata.json";
}

} // namespace

VectorStore::VectorStore()
{
    load_from_disk();
}

void VectorStore::clear()
{
    records_.clear();
    document_ids_.clear();
    persist_to_disk();
}

void VectorStore::add_document(const std::string& document_id,
                               const std::vector<std::string>& chunks,
                               const EmbeddingGenerator& embedding_generator)
{
    if (document_id.empty() || chunks.empty()) {
        return;
    }

    remove_document_if_exists(document_id);
    const std::string source_file = extract_source_file(document_id);

    std::size_t added = 0;
    for (std::size_t chunk_index = 0; chunk_index < chunks.size(); ++chunk_index) {
        const std::string& chunk = chunks[chunk_index];
        if (chunk.empty()) {
            continue;
        }

        VectorRecord record;
        record.document_id = document_id;
        record.chunk_id = chunk_index + 1;
        record.source_file = source_file;
        record.chunk_text = chunk;
        record.embedding = embedding_generator.generate_embedding(chunk);

        records_.push_back(std::move(record));
        ++added;
    }

    if (added > 0U) {
        document_ids_.push_back(document_id);
    }

    persist_to_disk();
}

std::vector<SearchResult> VectorStore::search(const std::vector<float>& query_embedding,
                                              std::size_t k) const
{
    if (query_embedding.empty() || records_.empty() || k == 0) {
        return {};
    }

    std::vector<std::tuple<float, std::size_t>> scored_indices;
    scored_indices.reserve(records_.size());

    for (std::size_t i = 0; i < records_.size(); ++i) {
        const float score = cosine_similarity(query_embedding, records_[i].embedding);
        scored_indices.emplace_back(score, i);
    }

    const std::size_t top_n = std::min(k, scored_indices.size());
    std::partial_sort(
        scored_indices.begin(),
        scored_indices.begin() + static_cast<std::ptrdiff_t>(top_n),
        scored_indices.end(),
        [](const auto& left, const auto& right) {
            return std::get<0>(left) > std::get<0>(right);
        });

    std::vector<SearchResult> results;
    results.reserve(top_n);

    for (std::size_t rank = 0; rank < top_n; ++rank) {
        const std::size_t index = std::get<1>(scored_indices[rank]);
        const auto& record = records_[index];

        SearchResult result;
        result.document_id = record.document_id;
        result.chunk_id = record.chunk_id;
        result.source_file = record.source_file;
        result.chunk_text = record.chunk_text;
        result.score = std::get<0>(scored_indices[rank]);
        results.push_back(std::move(result));
    }

    return results;
}

bool VectorStore::empty() const
{
    return records_.empty();
}

std::size_t VectorStore::document_count() const
{
    return document_ids_.size();
}

std::size_t VectorStore::chunk_count() const
{
    return records_.size();
}

float VectorStore::cosine_similarity(const std::vector<float>& a, const std::vector<float>& b)
{
    if (a.empty() || b.empty() || a.size() != b.size()) {
        return 0.0f;
    }

    float dot = 0.0f;
    float norm_a = 0.0f;
    float norm_b = 0.0f;

    for (std::size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
    }

    if (norm_a <= 0.0f || norm_b <= 0.0f) {
        return 0.0f;
    }

    return dot / (std::sqrt(norm_a) * std::sqrt(norm_b));
}

void VectorStore::load_from_disk()
{
    records_.clear();
    document_ids_.clear();

    const std::filesystem::path metadata_path = metadata_file_path();
    const std::filesystem::path chunks_path = chunks_file_path();
    const std::filesystem::path embeddings_path = embeddings_file_path();

    if (!std::filesystem::exists(metadata_path) ||
        !std::filesystem::exists(chunks_path) ||
        !std::filesystem::exists(embeddings_path)) {
        return;
    }

    try {
        std::ifstream metadata_stream(metadata_path);
        std::ifstream chunks_stream(chunks_path);
        std::ifstream embeddings_stream(embeddings_path, std::ios::binary);

        if (!metadata_stream || !chunks_stream || !embeddings_stream) {
            records_.clear();
            document_ids_.clear();
            return;
        }

        json metadata_json;
        json chunks_json;

        metadata_stream >> metadata_json;
        chunks_stream >> chunks_json;

        const std::size_t chunk_count =
            metadata_json.value("chunk_count", static_cast<std::size_t>(0));
        const std::size_t embedding_dimension =
            metadata_json.value("embedding_dimension", static_cast<std::size_t>(0));

        if (!chunks_json.is_array() ||
            chunks_json.size() != chunk_count ||
            embedding_dimension == 0U) {
            records_.clear();
            document_ids_.clear();
            return;
        }

        const std::size_t total_values = chunk_count * embedding_dimension;
        std::vector<float> embedding_buffer(total_values, 0.0f);

        const std::size_t bytes_to_read = total_values * sizeof(float);
        if (bytes_to_read > 0U) {
            embeddings_stream.read(
                reinterpret_cast<char*>(embedding_buffer.data()),
                static_cast<std::streamsize>(bytes_to_read));

            if (static_cast<std::size_t>(embeddings_stream.gcount()) != bytes_to_read) {
                records_.clear();
                document_ids_.clear();
                return;
            }
        }

        records_.reserve(chunk_count);
        for (std::size_t i = 0; i < chunk_count; ++i) {
            const json& chunk_json = chunks_json.at(i);

            VectorRecord record;
            record.document_id = chunk_json.value("document_id", std::string());
            record.chunk_id = chunk_json.value("chunk_id", static_cast<std::size_t>(0));
            record.source_file = chunk_json.value("source_file", std::string());
            record.chunk_text = chunk_json.value("chunk_text", std::string());

            const std::size_t offset = i * embedding_dimension;
            record.embedding.assign(
                embedding_buffer.begin() + static_cast<std::ptrdiff_t>(offset),
                embedding_buffer.begin() +
                    static_cast<std::ptrdiff_t>(offset + embedding_dimension));

            if (!record.document_id.empty() &&
                !record.chunk_text.empty() &&
                !record.embedding.empty()) {
                records_.push_back(std::move(record));
            }
        }

        if (metadata_json.contains("document_ids") &&
            metadata_json["document_ids"].is_array()) {
            document_ids_ = metadata_json["document_ids"].get<std::vector<std::string>>();
        } else {
            std::set<std::string> unique_documents;
            for (const auto& record : records_) {
                if (!record.document_id.empty() &&
                    unique_documents.insert(record.document_id).second) {
                    document_ids_.push_back(record.document_id);
                }
            }
        }
    } catch (...) {
        records_.clear();
        document_ids_.clear();
    }
}

void VectorStore::persist_to_disk() const
{
    const std::filesystem::path directory = index_directory();
    std::filesystem::create_directories(directory);

    const std::filesystem::path embeddings_path = embeddings_file_path();
    const std::filesystem::path chunks_path = chunks_file_path();
    const std::filesystem::path metadata_path = metadata_file_path();

    {
        std::ofstream embeddings_stream(
            embeddings_path,
            std::ios::binary | std::ios::trunc);

        for (const auto& record : records_) {
            if (record.embedding.empty()) {
                continue;
            }

            embeddings_stream.write(
                reinterpret_cast<const char*>(record.embedding.data()),
                static_cast<std::streamsize>(record.embedding.size() * sizeof(float)));
        }
    }

    {
        json chunks_json = json::array();
        for (const auto& record : records_) {
            chunks_json.push_back({
                {"document_id", record.document_id},
                {"chunk_id", record.chunk_id},
                {"source_file", record.source_file},
                {"chunk_text", record.chunk_text},
            });
        }

        std::ofstream chunks_stream(chunks_path, std::ios::trunc);
        chunks_stream << chunks_json.dump(2);
    }

    {
        json metadata_json;
        metadata_json["version"] = 2;
        metadata_json["index_path"] = directory.string();
        metadata_json["embedding_model"] = "bge-small-en-v1.5";
        metadata_json["embedding_dimension"] =
            records_.empty() ? 0U : records_.front().embedding.size();
        metadata_json["document_count"] = document_ids_.size();
        metadata_json["chunk_count"] = records_.size();
        metadata_json["document_ids"] = document_ids_;

        std::ofstream metadata_stream(metadata_path, std::ios::trunc);
        metadata_stream << metadata_json.dump(2);
    }
}

void VectorStore::remove_document_if_exists(const std::string& document_id)
{
    const auto doc_it =
        std::find(document_ids_.begin(), document_ids_.end(), document_id);

    if (doc_it == document_ids_.end()) {
        return;
    }

    records_.erase(
        std::remove_if(
            records_.begin(),
            records_.end(),
            [&](const VectorRecord& record) {
                return record.document_id == document_id;
            }),
        records_.end());

    document_ids_.erase(doc_it);
}

} // namespace zipper::rag
