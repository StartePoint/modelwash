#pragma once

#include "modelwash/document.hpp"
#include "modelwash/wash.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace modelwash {

struct ImportRequest {
  std::filesystem::path path;
};

struct ImportResult {
  bool ok = false;
  Document document;
  std::string messageZh;
};

class IFormatAdapter {
 public:
  virtual ~IFormatAdapter() = default;
  virtual std::string formatId() const = 0;
  virtual bool canOpen(const std::filesystem::path& path) const = 0;
  virtual ImportResult importFile(const ImportRequest& req) const = 0;
};

class FormatRegistry {
 public:
  void add(std::unique_ptr<IFormatAdapter> adapter);
  ImportResult open(const std::filesystem::path& path) const;

 private:
  std::vector<std::unique_ptr<IFormatAdapter>> adapters_;
};

std::unique_ptr<IFormatAdapter> makeAssimpAdapter();

struct ExportRequest {
  const Document* document = nullptr;
  std::filesystem::path output;
  GeomCompression compression = GeomCompression::None;
};

struct ExportResult {
  bool ok = false;
  std::uint64_t bytesWritten = 0;
  std::string messageZh;
};

ExportResult exportGlb(const ExportRequest& req);

}  // namespace modelwash
