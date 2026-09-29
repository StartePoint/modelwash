#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace modelwash {

using NodeId = std::uint64_t;
using MaterialId = std::uint64_t;
using TextureId = std::uint64_t;

struct Vec2 {
  float x = 0, y = 0;
};
struct Vec3 {
  float x = 0, y = 0, z = 0;
};
struct Vec4 {
  float x = 0, y = 0, z = 0, w = 1;
};
struct Quat {
  float x = 0, y = 0, z = 0, w = 1;
};

struct TRS {
  Vec3 translation{};
  Quat rotation{};
  Vec3 scale{1, 1, 1};
};

enum class MeshFlags : std::uint32_t {
  None = 0,
  Skinned = 1u << 0,
  HasMorph = 1u << 1,
};

inline MeshFlags operator|(MeshFlags a, MeshFlags b) {
  return static_cast<MeshFlags>(static_cast<std::uint32_t>(a) |
                                static_cast<std::uint32_t>(b));
}
inline bool hasFlag(MeshFlags f, MeshFlags bit) {
  return (static_cast<std::uint32_t>(f) & static_cast<std::uint32_t>(bit)) != 0;
}

struct MeshData {
  std::vector<Vec3> positions;
  std::vector<Vec3> normals;
  std::vector<Vec2> uvs;
  std::vector<std::uint32_t> indices;
  MeshFlags flags = MeshFlags::None;

  std::uint64_t triangleCount() const {
    return indices.empty() ? positions.size() / 3 : indices.size() / 3;
  }
};

struct Texture {
  TextureId id = 0;
  std::string name;
  int width = 0;
  int height = 0;
  int channels = 4;
  std::vector<std::uint8_t> pixels;
};

struct Material {
  MaterialId id = 0;
  std::string name;
  Vec4 baseColorFactor{1, 1, 1, 1};
  float metallic = 0.f;
  float roughness = 0.5f;
  std::optional<TextureId> baseColorTexture;
};

struct SceneNode {
  NodeId id = 0;
  std::string name;
  bool visible = true;
  TRS localTransform{};
  std::optional<MeshData> mesh;
  std::optional<MaterialId> materialId;
};

enum class WarningCode {
  None = 0,
  SoftFaceBudget,
  StrongFaceBudget,
  SkippedSkinnedSimplify,
  ImportPartial,
};

struct DocumentMeta {
  std::string sourcePath;
  std::vector<WarningCode> warnings;
  bool hasAnimation = false;
  bool hasSkinning = false;
  bool hasMorph = false;
  std::uint64_t triangleCount = 0;
  std::uint64_t approxTextureBytes = 0;
};

}  // namespace modelwash
