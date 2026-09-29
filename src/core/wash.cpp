#include "modelwash/wash.hpp"

#include <meshoptimizer.h>

#include <algorithm>
#include <cmath>

namespace modelwash {
namespace {

void simplifyMesh(MeshData& mesh, float ratio) {
  if (mesh.indices.size() < 3 || mesh.positions.empty()) {
    return;
  }
  ratio = std::clamp(ratio, 0.05f, 1.f);
  const std::size_t indexCount = mesh.indices.size();
  const std::size_t vertexCount = mesh.positions.size();
  const std::size_t targetIndexCount =
      std::max<std::size_t>(3, static_cast<std::size_t>(indexCount * ratio) / 3 * 3);

  std::vector<unsigned int> outIndices(indexCount);
  const float* positions = &mesh.positions[0].x;
  const std::size_t resultCount = meshopt_simplify(
      outIndices.data(), mesh.indices.data(), indexCount, positions, vertexCount,
      sizeof(Vec3), targetIndexCount, 1e-2f, 0, nullptr);
  outIndices.resize(resultCount);

  std::vector<Vec3> newPos(vertexCount);
  std::vector<Vec3> newNrm;
  std::vector<Vec2> newUv;
  if (!mesh.normals.empty()) {
    newNrm.resize(vertexCount);
  }
  if (!mesh.uvs.empty()) {
    newUv.resize(vertexCount);
  }

  std::vector<unsigned int> remap(vertexCount);
  const std::size_t newVertexCount = meshopt_optimizeVertexFetchRemap(
      remap.data(), outIndices.data(), outIndices.size(), vertexCount);

  meshopt_remapIndexBuffer(outIndices.data(), outIndices.data(), outIndices.size(),
                           remap.data());
  meshopt_remapVertexBuffer(newPos.data(), mesh.positions.data(), vertexCount,
                            sizeof(Vec3), remap.data());
  if (!newNrm.empty()) {
    meshopt_remapVertexBuffer(newNrm.data(), mesh.normals.data(), vertexCount,
                              sizeof(Vec3), remap.data());
  }
  if (!newUv.empty()) {
    meshopt_remapVertexBuffer(newUv.data(), mesh.uvs.data(), vertexCount,
                              sizeof(Vec2), remap.data());
  }

  newPos.resize(newVertexCount);
  if (!newNrm.empty()) {
    newNrm.resize(newVertexCount);
  }
  if (!newUv.empty()) {
    newUv.resize(newVertexCount);
  }

  mesh.positions = std::move(newPos);
  mesh.normals = std::move(newNrm);
  mesh.uvs = std::move(newUv);
  mesh.indices = outIndices;
}

void downscaleTexture(Texture& tex, std::uint32_t maxEdge) {
  if (tex.width <= 0 || tex.height <= 0 || tex.pixels.empty()) {
    return;
  }
  const int maxDim = std::max(tex.width, tex.height);
  if (static_cast<std::uint32_t>(maxDim) <= maxEdge) {
    return;
  }
  const float scale = static_cast<float>(maxEdge) / static_cast<float>(maxDim);
  const int nw = std::max(1, static_cast<int>(tex.width * scale));
  const int nh = std::max(1, static_cast<int>(tex.height * scale));
  const int ch = std::max(1, tex.channels);
  std::vector<std::uint8_t> out(static_cast<std::size_t>(nw * nh * ch), 0);

  for (int y = 0; y < nh; ++y) {
    for (int x = 0; x < nw; ++x) {
      const int sx = std::min(tex.width - 1, x * tex.width / nw);
      const int sy = std::min(tex.height - 1, y * tex.height / nh);
      for (int c = 0; c < ch; ++c) {
        out[(y * nw + x) * ch + c] =
            tex.pixels[(sy * tex.width + sx) * ch + c];
      }
    }
  }
  tex.width = nw;
  tex.height = nh;
  tex.pixels = std::move(out);
}

}  // namespace

WashSettings presetFor(WashTarget target) {
  WashSettings s;
  s.target = target;
  switch (target) {
    case WashTarget::Web:
      s.simplifyRatio = 0.5f;
      s.maxTextureEdge = 1024;
      s.geomCompression = GeomCompression::Meshopt;
      s.weldVertices = true;
      break;
    case WashTarget::Game:
      s.simplifyRatio = 0.7f;
      s.maxTextureEdge = 2048;
      s.geomCompression = GeomCompression::Draco;
      s.weldVertices = false;
      break;
    case WashTarget::Custom:
      break;
  }
  return s;
}

WashResult runWash(const Document& input, const WashSettings& settings) {
  WashResult result;
  result.document = input.clone();
  result.stats.trianglesBefore = input.meta().triangleCount;
  result.stats.textureBytesBefore = input.meta().approxTextureBytes;

  bool skippedSkinned = false;
  for (auto& node : result.document.nodes()) {
    if (!node.mesh) {
      continue;
    }
    const bool protectedMesh =
        hasFlag(node.mesh->flags, MeshFlags::Skinned) ||
        hasFlag(node.mesh->flags, MeshFlags::HasMorph) ||
        result.document.meta().hasAnimation;

    if (settings.simplifyGeometry && protectedMesh) {
      skippedSkinned = true;
      continue;
    }
    if (settings.simplifyGeometry) {
      simplifyMesh(*node.mesh, settings.simplifyRatio);
    }
  }

  for (auto& tex : result.document.textures()) {
    downscaleTexture(tex, settings.maxTextureEdge);
  }

  result.document.recomputeStats();
  result.stats.trianglesAfter = result.document.meta().triangleCount;
  result.stats.textureBytesAfter = result.document.meta().approxTextureBytes;
  // Rough estimate until exporter measures exact bytes.
  result.stats.estimatedFileBytes =
      result.stats.trianglesAfter * 48 + result.stats.textureBytesAfter;

  if (skippedSkinned) {
    result.warnings.push_back(WarningCode::SkippedSkinnedSimplify);
    result.messageZh =
        "Detected skinned/morph/animation; skipped decimation.";
  } else {
    result.messageZh = "Wash preview done.";
  }
  return result;
}

}  // namespace modelwash
