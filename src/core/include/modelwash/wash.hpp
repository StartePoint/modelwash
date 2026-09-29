#pragma once

#include "modelwash/document.hpp"

#include <string>
#include <vector>

namespace modelwash {

enum class WashTarget { Web, Game, Custom };

enum class TexCodec { Keep, Jpeg, WebP };
enum class GeomCompression { None, Draco, Meshopt };

struct WashSettings {
  WashTarget target = WashTarget::Web;
  bool simplifyGeometry = true;
  float simplifyRatio = 0.5f;
  std::uint32_t maxTextureEdge = 1024;
  TexCodec texCodec = TexCodec::Keep;
  int texQuality = 80;
  bool pruneUnused = true;
  bool weldVertices = false;
  GeomCompression geomCompression = GeomCompression::Meshopt;
  int compressionLevel = 5;
};

struct WashStats {
  std::uint64_t trianglesBefore = 0;
  std::uint64_t trianglesAfter = 0;
  std::uint64_t textureBytesBefore = 0;
  std::uint64_t textureBytesAfter = 0;
  std::uint64_t estimatedFileBytes = 0;
};

struct WashResult {
  Document document;
  WashStats stats;
  std::vector<WarningCode> warnings;
  std::string messageZh;
};

WashSettings presetFor(WashTarget target);
WashResult runWash(const Document& input, const WashSettings& settings);

}  // namespace modelwash
