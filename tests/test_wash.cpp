#include "modelwash/document.hpp"
#include "modelwash/wash.hpp"

#include <cstdio>

int main() {
  using namespace modelwash;
  Document doc;
  SceneNode node;
  node.name = "box";
  MeshData mesh;
  // Simple quad as two triangles, duplicated to give meshoptimizer something to do
  for (int i = 0; i < 100; ++i) {
    const float f = static_cast<float>(i) * 0.01f;
    mesh.positions.push_back({f, 0, 0});
    mesh.positions.push_back({f, 1, 0});
    mesh.positions.push_back({f + 0.01f, 0, 0});
    const auto base = static_cast<std::uint32_t>(mesh.positions.size() - 3);
    mesh.indices.push_back(base);
    mesh.indices.push_back(base + 1);
    mesh.indices.push_back(base + 2);
  }
  node.mesh = mesh;
  doc.addNode(std::move(node));
  doc.recomputeStats();

  const auto before = doc.meta().triangleCount;
  auto settings = presetFor(WashTarget::Web);
  settings.simplifyRatio = 0.5f;
  const WashResult wr = runWash(doc, settings);
  if (wr.stats.trianglesAfter == 0 || wr.stats.trianglesAfter > before) {
    std::printf("wash failed: before=%llu after=%llu\n",
                (unsigned long long)before,
                (unsigned long long)wr.stats.trianglesAfter);
    return 1;
  }
  std::printf("ok before=%llu after=%llu\n",
              (unsigned long long)before,
              (unsigned long long)wr.stats.trianglesAfter);
  return 0;
}
